#include "TcpEopll.hpp"

#include "../file_opt/Log.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>


namespace zyb
{

    IMPLEMENT_SINGLETON(CTcpEpoll)
    {
        m_nKeepliveIdle     = 0; // 距离上次传送数据多少时间未收到判断为开始检测
        m_nKeepliveInterval = 0; // 检测开始每多少时间发送心跳包
        m_bKeepliveCount    = 0; // 发送几次心跳包对方未响应则close连接
        
        // 关闭TCP Nagle 算法
        m_bCloseTcpNagle    = false;
        m_sListenSocket      = INVALID_SOCKET;
    } 
    
    CTcpEpoll::~CTcpEpoll()
    {
        if (INVALID_SOCKET != m_sListenSocket)
        {
            close(m_sListenSocket);
            m_sListenSocket = INVALID_SOCKET;
        }
    }
    
    bool CTcpEpoll::InitTcpEpoll(const char *pIp, unsigned short usPort, EpollMsgCallbackFun pEpollMsgFun,
        int nKeepIdle/* = 0*/, int nKeepInterval/* = 0*/, int nKeepCount/* = 0*/,
        bool bCloseTcpNagle/* = false*/)
    {
        bool bRet = false;
        for (;;)
        {
            if (!CEpoll::InitEpoll(pEpollMsgFun, enumERT_TCP))
            {
                break;
            }
            
            if (
                (0 == nKeepIdle && 0 == nKeepInterval && 0 == nKeepCount) ||
                (0 < nKeepIdle && 0 < nKeepInterval && 0 < nKeepCount)    
               )
            {
                m_nKeepliveIdle     = nKeepIdle;
                m_nKeepliveInterval = nKeepInterval;
                m_bKeepliveCount    = nKeepCount;
            }
            else
            {
                LOG_ERROR("keeplive参数错误");
                break;
            }
            m_bCloseTcpNagle    = bCloseTcpNagle;
            
            if (!InitListenSocket(pIp, usPort))
            {
                break;
            }
            if (!SetSocketNonblacking(m_sListenSocket))
            {
                break;
            }
            if (0 != listen(m_sListenSocket, SOMAXCONN)) 
            {
                LOG_ERROR("listen socket 失败");
                break;
            }
            
            // Create epoll
            if (!CreateEpoll())
            {
                break;
            }
            if (!EpollCtrl(EPOLL_CTL_ADD, EPOLLIN | EPOLLET, m_sListenSocket))
            {
                break;
            }
  
            m_listenSocket = m_sListenSocket;
            m_bInit = true;
            bRet = true;
            break;
        }
        return bRet;
    }
    
    
    SOCKET_FD CTcpEpoll::Connect(const char *pIp, unsigned short usPort)
    {
        SOCKET_FD fd = INVALID_SOCKET_FD;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("没有初始化");
                break;
            }
            if (!pIp || 0 == pIp[0])
            {
                LOG_ERROR("IP is NULL");
                break;
            }
            SOCKET sCli = socket(AF_INET, SOCK_STREAM, 0);
            if (-1 == sCli) 
            {
                LOG_ERROR("创建socket 失败");
                break;
            }
            
            struct sockaddr_in srvAddr;
            srvAddr.sin_family = AF_INET;
            srvAddr.sin_addr.s_addr = inet_addr(pIp);
            srvAddr.sin_port = htons(usPort);
            if (0 != connect(sCli, (sockaddr *) & srvAddr, sizeof (srvAddr)))
            {
                LOG_ERROR("connect 失败");
                close(sCli);
                break;
            }
            fd = AddSocketToEpoll(sCli);
            if (INVALID_SOCKET_FD == fd)
            {
                close(sCli);
                break;
            }
            m_pCallbackFun(enumTCP_EM_CONNECT, fd, 0, 0);
            break;
        }
        return fd;
    }

    bool CTcpEpoll::InitListenSocket(const char *pIp, unsigned short usPort)
    {
        bool bRet = false;
        for (;;)
        {
            m_sListenSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (-1 == m_sListenSocket)
            {
                LOG_ERROR("创建socket 失败");
                break;
            }
            // 一个端口释放后会等待两分钟之后才能再被使用，SO_REUSEADDR是让端口释放后立即就可以被再次使用
            int on = 1;
            if (0 != setsockopt(m_sListenSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on))) 
            {
                LOG_ERROR("set socket SO_REUSEADDR 失败");
                break;
            }
            struct sockaddr_in localAddr;
            localAddr.sin_family = AF_INET;
            if (!pIp || !pIp[0])
            {
                localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            }
            else
            {
                localAddr.sin_addr.s_addr = inet_addr(pIp);
            }
            
            localAddr.sin_port = htons(usPort);
            if (0 != bind(m_sListenSocket, (struct sockaddr *) &localAddr, sizeof (localAddr)))
            {
                LOG_ERROR("bind socket 失败");
                close(m_sListenSocket);
                m_sListenSocket = 0;
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    SOCKET_FD CTcpEpoll::AddSocketToEpoll(SOCKET nSocket)
    {
        SOCKET_FD fd = INVALID_SOCKET_FD;
        for (;;)
        {
            if (!SetSocketNonblacking(nSocket))
            {
                break;
            }
            
            // close Nagle算法
            if (m_bCloseTcpNagle)
            {
                int flag = 1;
                if (0 != setsockopt(nSocket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof (flag)))
                {
                    LOG_ERROR("close Nagle算法失败 socket=%d", nSocket);
                    break;
                }
            }
            if (m_nKeepliveIdle > 0 && m_nKeepliveInterval > 0 && m_bKeepliveCount > 0) 
            {
                // keep-live
                int keepAlive = 1; // 开启keepalive属性. 缺省值: 0(关闭)   
                setsockopt(nSocket, SOL_SOCKET, SO_KEEPALIVE, (void*) &keepAlive, sizeof (keepAlive));
                setsockopt(nSocket, SOL_TCP, TCP_KEEPIDLE, (void*) &m_nKeepliveIdle, sizeof (m_nKeepliveIdle));
                setsockopt(nSocket, SOL_TCP, TCP_KEEPINTVL, (void*) &m_nKeepliveInterval, sizeof (m_nKeepliveInterval));
                setsockopt(nSocket, SOL_TCP, TCP_KEEPCNT, (void*) &m_bKeepliveCount, sizeof (m_bKeepliveCount));
            }
            
            if (!EpollCtrl(EPOLL_CTL_ADD, EPOLLHUP | EPOLLERR | EPOLLIN | EPOLLET, nSocket))
            {
                break;
            }
            
            fd = this ->RelationSocketFd(nSocket);
            if (INVALID_SOCKET_FD == fd)
            {
                epoll_ctl(m_epollFd, EPOLL_CTL_DEL, nSocket, NULL);
                break;
            }
            break;
        }
        return fd;
    }
    
    void CTcpEpoll::ProcessAccpet()
    {
        while (1)
        {
            struct sockaddr_in cliAddr;
            socklen_t sockLen = sizeof(cliAddr);
            int nCliSocket = accept(m_sListenSocket, (sockaddr *)&cliAddr, &sockLen);
            if (-1 == nCliSocket)
            {
                if (EINTR == errno || EWOULDBLOCK == errno)
                {
                    break;
                }
                if (EMFILE == errno)
                {
                    LOG_ERROR("socket 打开文件数已满， 可能遭受攻击");
                    break;
                }
                continue;
            }
            
            SOCKET_FD fd = AddSocketToEpoll(nCliSocket);
            if (INVALID_SOCKET_FD == fd)
            {
                close(nCliSocket);
                continue;
            }
            char szIpTemp[64] = {0};
            snprintf(szIpTemp, 64, "%s:%hu", inet_ntoa(cliAddr.sin_addr), cliAddr.sin_port);
            m_pCallbackFun(enumTCP_EM_ACCEPT, fd, szIpTemp, 64);
        }
    }
}