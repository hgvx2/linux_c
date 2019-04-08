#ifndef UDP_SRV_HPP
#define UDP_SRV_HPP
#include "base/UdpSrvBaseMsg.hpp"
#include "../../epoll/SocketDefault.hpp"
#include "../../lock/Lock.hpp"
#include <map>

namespace zyb
{
    class CUdpSrvBase;
    class CThreadPool;
    class CBinaryMemPool;
    class CTask;
    class CUdpSrvTaskManager;
    class CBinaryBuf;
    
    class CUdpSrv
    {
    public :
        CUdpSrv();
        virtual ~CUdpSrv();
        
        bool InitUdpSrv(const char *pIp, unsigned short usPort);
        
        bool SendData(SOCKET_FD fd, const char *pBuf, int nSize);
        
        SOCKET_FD Connect(const char *pIp, unsigned short usPort);
        
        void CloseSocketFD(SOCKET_FD fd);
        
        void RunTcpSrv();
        
        bool PushTaskToBack(CTask *pTask);
        bool PushTaskToFront(CTask *pTask);
        
    private:
        static bool OnUdpSrvBaseCallback(UDP_SRV_BASE_MSG msg, SOCKET_FD fd, const char * pData, int nSize);
        static bool OnUdpSrvTaskCallback(CTask *pTask);
        
        bool ProcPack(SOCKET_FD fd);
        bool ProcValidate(SOCKET_FD fd, const char * pData, int nSize);
        
        virtual bool ProcAcceptFD(SOCKET_FD fd) = 0;
        virtual bool ProcConnectFD(SOCKET_FD fd) = 0;
        virtual bool ProcDisableFD(SOCKET_FD fd) = 0;
        virtual bool ProcPack(SOCKET_FD fd, const char * pData, int nSize) = 0;

    private:
        CUdpSrvBase  *m_pUdpSrvBase;
        CThreadPool  *m_pThreadPool;
        CUdpSrvTaskManager *m_pTaskManager;

        CBinaryMemPool *m_pBinaryMemPool;
        
        bool m_bInit;
        
        static CUdpSrv *m_pThis;
    };
}

#endif //UDP_SRV_HPP