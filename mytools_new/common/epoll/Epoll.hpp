#ifndef EPOLL_HPP
#define EPOLL_HPP

#include "SocketDefault.hpp"
#include "../lock/Lock.hpp"

#include <sys/epoll.h>
#include <map>

namespace zyb
{
#define LISTEN_MAX_EVENT  (1024)
#define MAX_RECV_LEN      (64 * 1024)
    
    typedef enum _EPOLL_MSG
    {
        enumUDP_EM_VALIDATE   = 1,
        enumUDP_EM_ACCEPT     = 2,
        enumUDP_EM_RECV       = 3,
        enumUDP_EM_DISCONNECT = 4,
        enumUDP_EM_SEND       = 5,
        enumUDP_EM_CONNECT    = 6,
                
        enumTCP_EM_ACCEPT     = 7,
        enumTCP_EM_CONNECT    = 8,
        enumTCP_EM_DISCONNECT = 9,
        enumTCP_EM_RECV       = 10,
        enumTCP_EM_SEND       = 11,        
    }EPOLL_MSG;
    
    typedef enum _EPOLL_PROTOCOL_TYPE
    {
        enumERT_UNKNOW = 0,
        enumERT_UDP = 1,
        enumERT_TCP = 2,
    }EPOLL_PROTOCOL_TYPE;
    
    typedef bool (*EpollMsgCallbackFun)(EPOLL_MSG, SOCKET_FD, const char *, int);
    
    class CEpoll
    {
        friend class CUdpEpoll;
        friend class CTcpEpoll;
        CEpoll();
    public:
        virtual ~CEpoll();
        
        bool InitEpoll(EpollMsgCallbackFun pEpollMsgFun, EPOLL_PROTOCOL_TYPE protoType);
        
        void StartRun();
        void StopRun();
        
        void CloseSocketFd(SOCKET_FD fd);
        int SendDataFromFd(SOCKET_FD fd, const char *pBuf, int nBufSize);
    private:
        int SendDataFromSocket(SOCKET socket, const char *pBuf, int nBufSize);
        
        virtual void ProcessAccpet() = 0;
        void ProcessRecv(SOCKET nSocket, SOCKET_FD fd, char *pRecvBuf, unsigned int nRecvBufSize);
        
        bool CreateEpoll();
        bool EpollCtrl(int nOpt, unsigned int uEvent, SOCKET socket);
        bool SetSocketNonblacking(SOCKET socket);
        
        SOCKET_FD RelationSocketFd(SOCKET s, bool bConnect = false);
        SOCKET_FD FindSocketFd(SOCKET s);
        SOCKET FindSocket(SOCKET_FD fd);
        void DeleteSocket(SOCKET_FD fd, SOCKET s);
    private:
        EpollMsgCallbackFun m_pCallbackFun;
        bool m_bInit;
        EPOLL_FD m_epollFd;
        
        SOCKET m_listenSocket;
        
        SOCKET_FD m_nCliSocketFd;   // 客户端连接过来的socket 配置从10000 以后开始
        SOCKET_FD m_nConnectFd;     // 服务器主动连接的socket 配置从1 - 10000之间
        
        struct epoll_event m_event[LISTEN_MAX_EVENT];
        
        CShareRWLock m_socketShareLock;
        std::map<SOCKET_FD, SOCKET> m_mapFdSocket;
        std::map<SOCKET, SOCKET_FD> m_mapSocketFd;
        
        EPOLL_PROTOCOL_TYPE m_protocolType;
    };
}


#endif // EPOLL_HPP