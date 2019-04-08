#ifndef UDP_EPOLL_HPP
#define UDP_EPOLL_HPP

#include "Epoll.hpp"
#include "../design_patterns/Singleton.hpp"
#include <netinet/in.h>

namespace zyb
{
    class CUdpEpoll : public CEpoll
    {
        DECLARE_SINGLETON(CUdpEpoll)   
    public:
        ~CUdpEpoll();
        bool InitUdpEpoll(const char *pIp, unsigned short usPort, EpollMsgCallbackFun pEpollMsgFun);
        
    private:
        bool InitUdpLocalSocket(const char *pIp, unsigned short usPort);
        virtual void ProcessAccpet();
    private:
        SOCKET   m_sListenSocket;
        sockaddr_in m_localAddr;
    };
}


#endif // UDP_EPOLL_HPP