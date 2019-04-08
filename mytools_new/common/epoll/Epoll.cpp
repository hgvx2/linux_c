#include "Epoll.hpp"
#include "../file_opt/Log.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
namespace zyb
{
    #define MAX_TIME_OUT       (-1) // INFITE timeout
    #define MIN_CON_SOCKET_FD  (1)
    #define MAX_CON_SOCKET_FD  (10000)
    #define MIN_CLI_SOCKET_FD  (10001)
    #define MAX_CLI_SOCKET_FD  ((unsigned int)(-1)) 

    
    CEpoll::CEpoll()
    {
        m_protocolType = enumERT_UNKNOW;
        m_pCallbackFun = 0;
        m_bInit        = false;
        m_epollFd      = 0;
        m_nCliSocketFd = MIN_CLI_SOCKET_FD;
        m_nConnectFd   = MIN_CON_SOCKET_FD;
        m_listenSocket = INVALID_SOCKET;
        
        memset(m_event, 0, LISTEN_MAX_EVENT * sizeof(epoll_event));
        m_mapFdSocket.clear();
        m_mapSocketFd.clear();
    }
    
    CEpoll::~CEpoll()
    {
        m_bInit             = false;
        m_pCallbackFun      = 0;
        if (m_epollFd)
        {
            close(m_epollFd);
            m_epollFd = 0;
        }
        
        CAutoShareReadLock _lock(&m_socketShareLock, CAutoShareReadLock::emLT_WriteLock);
        m_mapFdSocket.clear();
        m_mapSocketFd.clear();
    }
    
    bool CEpoll::InitEpoll(EpollMsgCallbackFun pEpollMsgFun, EPOLL_PROTOCOL_TYPE protoType)
    {
        bool bRet = false;
        for (;;)
        {
            if (m_bInit)
            {
                LOG_ERROR("重复初始化");
                break;
            }
            if (!pEpollMsgFun)
            {
                LOG_ERROR("参数错误， 回掉函数为NULL");
                break;
            }
            m_pCallbackFun = pEpollMsgFun;
            
            if (enumERT_UDP == protoType || enumERT_TCP == protoType)
            {
                m_protocolType = protoType;
            }
            else
            {
                LOG_ERROR("协议类型错误 type=%d", protoType);
                break;
            }
            if (!CreateEpoll())
            {
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    void CEpoll::StartRun()
    {
        if (!m_bInit)
        {
            LOG_ERROR("没有初始化");
            return;
        }
        int nEvents = 0;
        SOCKET_FD fd;
        char szBufTemp[MAX_RECV_LEN];
        while (m_bInit)
        {
            nEvents = epoll_wait(m_epollFd, m_event, LISTEN_MAX_EVENT, MAX_TIME_OUT);
            for (int i = 0; i < nEvents; i ++)
            {
                fd = INVALID_SOCKET_FD;
                if (m_listenSocket != m_event[i].data.fd)
                {
                    fd = FindSocketFd(m_event[i].data.fd);
                    if (INVALID_SOCKET_FD == fd)
                    {
                        continue;
                    }
                }
                
                if (m_event[i].events & EPOLLERR || m_event[i].events & EPOLLHUP)
                {
                    // socket err
                    if (INVALID_SOCKET_FD != fd)
                    {
                        if (enumERT_UDP == m_protocolType)
                        {
                            m_pCallbackFun(enumUDP_EM_DISCONNECT, fd, 0, 0);
                        }
                        if (enumERT_TCP == m_protocolType)
                        {
                            m_pCallbackFun(enumTCP_EM_DISCONNECT, fd, 0, 0);
                        }
                    }
                    else
                    {
                        LOG_ERROR("socket 异常 但是没有找到socket 对于的FD socket=%d", m_event[i].data.fd);
                    }
                    continue;
                }
                if (m_event[i].events & EPOLLIN)
                {
                    if (m_listenSocket == m_event[i].data.fd)
                    {
                        ProcessAccpet();
                    } 
                    else 
                    {
                        ProcessRecv(m_event[i].data.fd, fd, szBufTemp, MAX_RECV_LEN);
                    }
                }
                if (m_event[i].events & EPOLLOUT)
                {
                    if (INVALID_SOCKET_FD != fd)
                    {
                        struct epoll_event ev;
                        ev.events = EPOLLHUP | EPOLLERR | EPOLLIN | EPOLLET;
                        ev.data.fd = m_event[i].data.fd;
                        if (-1 == epoll_ctl(m_epollFd, EPOLL_CTL_MOD, m_event[i].data.fd, &ev))
                        {
                            if (enumERT_UDP == m_protocolType)
                            {
                                m_pCallbackFun(enumUDP_EM_DISCONNECT, fd, 0, 0);
                            }
                            if (enumERT_TCP == m_protocolType)
                            {
                                m_pCallbackFun(enumTCP_EM_DISCONNECT, fd, 0, 0);
                            }
                            LOG_ERROR("epoll EPOLL_CTL_MOD 错误 socket=%d", m_event[i].data.fd);
                            break;
                        }
                        if (enumERT_UDP == m_protocolType)
                        {
                            m_pCallbackFun(enumUDP_EM_SEND, fd, 0, 0);
                        }
                        if (enumERT_TCP == m_protocolType)
                        {
                            m_pCallbackFun(enumTCP_EM_SEND, fd, 0, 0);
                        }
                    }
                    else
                    {
                        LOG_ERROR("没有找到socket 对于的FD socket=%d", m_event[i].data.fd);
                    }
                }
            }
        }
    }
    
    void CEpoll::StopRun()
    {
        m_bInit = false;
    }
    
    void CEpoll::CloseSocketFd(SOCKET_FD fd)
    {
        for (;;)
        {
            SOCKET s = FindSocket(fd);
            if (INVALID_SOCKET == s)
            {
                LOG_ERROR("没有找到fd对于的socket fd=%u", fd);
                break;
            }
            DeleteSocket(fd, s);
            if (0 != epoll_ctl(m_epollFd, EPOLL_CTL_DEL, s, NULL))
            {
                LOG_ERROR("从EPOLL 中清除socket=%d失败，可能早已删除", s);
            }
            close(s);
            break;
        }
    }
    
    int CEpoll::SendDataFromFd(SOCKET_FD fd, const char *pBuf, int nBufSize)
    {
        int nRet = -1;
        for (;;)
        {
            SOCKET nSocket = FindSocket(fd);
            if (INVALID_SOCKET == nSocket)
            {
                LOG_ERROR("没有找到指定的socket fd=%u", fd);
                break;
            }
            nRet = SendDataFromSocket(nSocket, pBuf, nBufSize);
            break;
        }
        return nRet;
    }
    
    int CEpoll::SendDataFromSocket(SOCKET socket, const char *pBuf, int nBufSize)
    {
        int nRet = -1;
        for (;;)
        {
            if (!pBuf || 0 >= nBufSize)
            {
                LOG_ERROR("参数错误 pbuf=%d size=%d", *((int*)pBuf), nBufSize);
                break;
            }
            if (!m_bInit)
            {
                LOG_ERROR("没有初始化");
                break;
            }
            
            struct epoll_event ev;
            ev.events = EPOLLHUP | EPOLLERR | EPOLLIN | EPOLLET;
            ev.data.fd = socket;
           
            int nSend = send(socket, pBuf, nBufSize, 0);
            if (0 > nSend)
            {
                if (errno == EINTR || errno == EWOULDBLOCK)
                {
                    ev.events |= EPOLLOUT;
                    if (-1 == epoll_ctl(m_epollFd, EPOLL_CTL_MOD, socket, &ev))
                    {
                        LOG_ERROR("epoll EPOLL_CTL_MOD 错误 socket=%d", socket);
                        break;
                    }
                    nRet = 0;
                }
                break;
            }
            if (0 == nSend)
            {
                break;
            }
            if (0 < nSend && nSend < nBufSize)
            {
                ev.events |= EPOLLOUT;
                if (-1 == epoll_ctl(m_epollFd, EPOLL_CTL_MOD, socket, &ev)) 
                {
                    LOG_ERROR("epoll EPOLL_CTL_MOD 错误 socket=%d", socket);
                    break;
                }
            }
            nRet = nSend;
            break;
        }
        return nRet;
    }
    
    void CEpoll::ProcessRecv(SOCKET nSocket, SOCKET_FD fd, char *pRecvBuf, unsigned int nRecvBufSize)
    {
        int nRecvRet;
        for (;;)
        {
            nRecvRet = recv(nSocket, pRecvBuf, nRecvBufSize, 0);
            if (0 > nRecvRet)
            {
                if (EINTR == errno || EWOULDBLOCK == errno)
                {
                    break;
                }
                break;
            }
            if (0 == nRecvRet)
            {
                break;
            }
            
            if (!m_pCallbackFun(enumERT_UDP == m_protocolType ? enumUDP_EM_RECV : enumTCP_EM_RECV, fd, pRecvBuf, nRecvRet))
            {
                break;
            }
            
            if (enumERT_TCP == m_protocolType && nRecvRet < MAX_RECV_LEN)
            {
                break;
            }
        }
    }
        
    bool CEpoll::CreateEpoll()
    {
       bool bRet = false;
       for (;;)
       {
           m_epollFd = epoll_create(LISTEN_MAX_EVENT);
            if (-1 == m_epollFd) 
            {
                LOG_ERROR("epoll_create err=%d", errno);
                break;
            }
           bRet = true;
           break;
       }
       return bRet;
    }
    
    bool CEpoll::EpollCtrl(int nOpt, unsigned int uEvent, SOCKET socket)
    {
        bool bRet = false;
        for (;;)
        {
            struct epoll_event ev;
            ev.events = uEvent;
            ev.data.fd = socket;
            if (-1 == epoll_ctl(m_epollFd, nOpt, socket, &ev))
            {
                LOG_ERROR("epoll_ctl err=%d socket=%d", errno, socket);
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CEpoll::SetSocketNonblacking(SOCKET socket)
    {
        bool bRet = false;
        for (;;)
        {
            int nGetFlags, nSetFlags;
            // Get file state flags
            nGetFlags = fcntl(socket, F_GETFL, 0);
            if (-1 == nGetFlags) 
            {
                break;
            }
            nGetFlags |= O_NONBLOCK;
            nSetFlags = fcntl(socket, F_SETFL, nGetFlags);
            if (-1 == nSetFlags) 
            {
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    SOCKET_FD CEpoll::RelationSocketFd(SOCKET s, bool bConnect/* = false*/)
    {
        SOCKET_FD fd = INVALID_SOCKET_FD;
        for (;;)
        {
            SOCKET_FD temp = 0;
            if (bConnect)
            {
                temp = m_nConnectFd ++;
                if (MAX_CON_SOCKET_FD <= m_nConnectFd)
                {
                    m_nConnectFd = MIN_CON_SOCKET_FD;
                }
            }
            else
            {
                temp = m_nCliSocketFd ++;
                if (MAX_CLI_SOCKET_FD <= m_nCliSocketFd)
                {
                    m_nCliSocketFd = MIN_CLI_SOCKET_FD;
                }
            }
                
            if (1)
            {
                CAutoShareReadLock _lock(&m_socketShareLock, CAutoShareReadLock::emLT_WriteLock);
                std::map<SOCKET, SOCKET_FD>::iterator mapSock = m_mapSocketFd.find(s);
                if (mapSock != m_mapSocketFd.end())
                {
                    LOG_ERROR("出现错误 此处不应该能搜到socket=%d", mapSock ->first);
                    break;
                }
                std::map<SOCKET_FD, SOCKET>::iterator mapFd = m_mapFdSocket.find(temp);
                if (mapFd != m_mapFdSocket.end())
                {                   
                    continue;
                }
                std::pair< std::map< SOCKET, SOCKET_FD >::iterator, bool > ret;
                ret = m_mapSocketFd.insert(std::pair<SOCKET, SOCKET_FD>(s, temp));
                if (!ret.second)
                {
                    LOG_ERROR("出现错误 添加socket=%d 对于的 fd=%u 失败", s, temp);
                    break;
                }
                
                std::pair< std::map< SOCKET_FD, SOCKET >::iterator, bool > retfd;
                retfd = m_mapFdSocket.insert(std::pair< SOCKET_FD, SOCKET >(temp, s));
                if (!retfd.second)
                {
                    m_mapSocketFd.erase(s);
                    LOG_ERROR("出现错误 添加socket=%d 对于的 fd=%u 失败", s, temp);
                    break;
                }
           
                fd = temp;
            }
           
            break;
        }
        return fd;
    }
    
    SOCKET_FD CEpoll::FindSocketFd(SOCKET s)
    {
        SOCKET_FD fdRet = INVALID_SOCKET_FD;
        for (;;)
        {
            CAutoShareReadLock _lock(&m_socketShareLock, CAutoShareReadLock::emLT_ReadLock);
            std::map<SOCKET, SOCKET_FD>::iterator mapSock = m_mapSocketFd.find(s);
            if (mapSock != m_mapSocketFd.end())
            {
                fdRet = mapSock ->second;
            }
            
            break;
        }
        return fdRet;
    }
    
    SOCKET CEpoll::FindSocket(SOCKET_FD fd)
    {
        SOCKET sRet = INVALID_SOCKET;
        for (;;)
        {
            CAutoShareReadLock _lock(&m_socketShareLock, CAutoShareReadLock::emLT_ReadLock);
            std::map<SOCKET_FD, SOCKET>::iterator mapFd = m_mapFdSocket.find(fd);
            if (mapFd != m_mapFdSocket.end())
            {
                sRet = mapFd ->second;
            }
            
            break;
        }
        return sRet;
    }
    
    void CEpoll::DeleteSocket(SOCKET_FD fd, SOCKET s)
    {
        CAutoShareReadLock _lock(&m_socketShareLock, CAutoShareReadLock::emLT_WriteLock);
        m_mapSocketFd.erase(s);
        m_mapFdSocket.erase(fd);
    }
}