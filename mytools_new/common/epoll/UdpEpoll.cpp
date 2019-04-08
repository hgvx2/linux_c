#include "UdpEpoll.hpp"
#include "../file_opt/Log.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

namespace zyb
{
    IMPLEMENT_SINGLETON(CUdpEpoll)
    {
        m_sListenSocket = INVALID_SOCKET;  
    }
    
    CUdpEpoll::~CUdpEpoll()
    {
        if (INVALID_SOCKET != m_sListenSocket)
        {
            close(m_sListenSocket);
            m_sListenSocket = INVALID_SOCKET;
        }
    }
    
    bool CUdpEpoll::InitUdpEpoll(const char *pIp, unsigned short usPort, EpollMsgCallbackFun pEpollMsgFun)
    {
        bool bRet = false;
        for (;;)
        {
            if (!CEpoll::InitEpoll(pEpollMsgFun, enumERT_UDP))
            {
                break;
            }
            if (!InitUdpLocalSocket(pIp, usPort))
            {
                break;
            }
            
            m_bInit = true;
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CUdpEpoll::InitUdpLocalSocket(const char *pIp, unsigned short usPort)
    {
        bool bRet = false;
        for (;;)
        {
            m_sListenSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
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
            m_localAddr.sin_family = AF_INET;
            m_localAddr.sin_port = htons(usPort);;
            if (!pIp || 0 == pIp[0])
            {
                m_localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            }
            else
            {
                m_localAddr.sin_addr.s_addr = inet_addr(pIp);
            }
            
            if (0 != bind(m_sListenSocket, (sockaddr*) &m_localAddr, sizeof(m_localAddr)))
            {
                LOG_ERROR("bind socket 失败 errcode=%d", errno);
                close(m_sListenSocket);
                m_sListenSocket = INVALID_SOCKET;
            }
            if (!SetSocketNonblacking(m_sListenSocket))
            {
                close(m_sListenSocket);
                m_sListenSocket = INVALID_SOCKET;
            }
            
            if (!CreateEpoll())
            {
                break;
            }

            if (!EpollCtrl(EPOLL_CTL_ADD, EPOLLIN | EPOLLET, m_sListenSocket))
            {
                break;
            }
            
            m_listenSocket = m_sListenSocket;
            bRet = true;
            break;
        }
        return bRet;
    }
   
    void CUdpEpoll::ProcessAccpet()
    {
        int nRecv;
        char szBuf[MAX_RECV_LEN];
        sockaddr_in cliAddr;
        socklen_t cliAddrLen = sizeof(cliAddr);
        SOCKET nNewCliSock = INVALID_SOCKET;
        SOCKET_FD nNewCliSockFd = INVALID_SOCKET_FD;
        for (;;)
        {
            if (INVALID_SOCKET != nNewCliSock)
            {
                close(nNewCliSock);
                nNewCliSock = INVALID_SOCKET;
            }
            nRecv = recvfrom(m_sListenSocket, szBuf, MAX_RECV_LEN, 0, (sockaddr *)&cliAddr, &cliAddrLen);
            if (0 >= nRecv)
            {
                break;
            }
            
            if (!m_pCallbackFun(enumUDP_EM_VALIDATE, INVALID_SOCKET_FD, szBuf, nRecv))
            {
                continue;
            }
            
            nNewCliSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (-1 == nNewCliSock)
            {
                LOG_ERROR("fd=%d   create socket error", nNewCliSock);
                continue;
            }
            // 
            int on = 1;
            if (0 != setsockopt(nNewCliSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof (on))) 
            {
                LOG_ERROR("set socket SO_REUSEADDR 失败");
                break;
            }
            if (0 != bind(nNewCliSock, (sockaddr*) &m_localAddr, sizeof(m_localAddr)))
            {
               LOG_ERROR("bind err=%d", errno);
               continue;
            }
            if (0 != connect(nNewCliSock, (sockaddr *)&cliAddr, cliAddrLen))
            {
                LOG_ERROR("fd=%d   connect socket error", nNewCliSock);
                continue;
            }
            if (!SetSocketNonblacking(nNewCliSock))
            {
                LOG_ERROR("fd=%d   SetSocketNonblacking err=%d", nNewCliSock, errno);
                continue;
            }
            nNewCliSockFd = RelationSocketFd(nNewCliSock);
            if (INVALID_SOCKET_FD == nNewCliSockFd)
            {
                continue;
            }
            struct epoll_event ev;
            ev.events = EPOLLHUP | EPOLLERR | EPOLLIN | EPOLLET;
            ev.data.fd = nNewCliSock;
            if (-1 == epoll_ctl(m_epollFd, EPOLL_CTL_ADD, nNewCliSock, &ev))
            {
                DeleteSocket(nNewCliSockFd, nNewCliSock);
                LOG_ERROR("添加socket到 epoll 失败");
                continue;
            }
            
            if (m_pCallbackFun(enumUDP_EM_ACCEPT, nNewCliSockFd, 0, 0))
            {
                nNewCliSock = INVALID_SOCKET;
            }
        }
    }
}