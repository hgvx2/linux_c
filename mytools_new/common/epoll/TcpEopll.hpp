#ifndef TCP_EPOLL_HPP
#define TCP_EPOLL_HPP

#include "../design_patterns/Singleton.hpp"
#include "Epoll.hpp"

namespace zyb
{
    class CTcpEpoll : public CEpoll
    {
        DECLARE_SINGLETON(CTcpEpoll)
    public:
        ~CTcpEpoll();
        
        bool InitTcpEpoll(const char *pIp, unsigned short usPort, EpollMsgCallbackFun pEpollMsgFun,
        int nKeepIdle = 0, int nKeepInterval = 0, int nKeepCount = 0,
        bool bCloseTcpNagle = false);
        
        SOCKET_FD Connect(const char *pIp, unsigned short usPort);

    private:
        bool InitListenSocket(const char *pIp, unsigned short usPort);
        SOCKET_FD AddSocketToEpoll(SOCKET nSocket);
        
        virtual void ProcessAccpet();
    private:
        // keeplive
        int m_nKeepliveIdle; // 距离上次传送数据多少时间未收到判断为开始检测
        int m_nKeepliveInterval; // 检测开始每多少时间发送心跳包
        int m_bKeepliveCount; // 发送几次心跳包对方未响应则close连接
        // 关闭TCP Nagle 算法
        bool m_bCloseTcpNagle;
        SOCKET m_sListenSocket;
    };
}

#endif // TCP_EPOLL_HPP