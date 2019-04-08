#ifndef TCP_SRV_HPP
#define TCP_SRV_HPP
#include "base/TcpSrvBaseMsg.hpp"
#include "../../epoll/SocketDefault.hpp"
#include "../../lock/Lock.hpp"
#include <map>

namespace zyb
{
    class CTcpSrvBase;
    class CThreadPool;
    class CBinaryMemPool;
    class CTask;
    class CTcpSrvTaskManager;
    class CBinaryBuf;
    
    class CTcpSrv
    {
    public :
        CTcpSrv();
        virtual ~CTcpSrv();
        
        bool InitTcpSrv(const char *pIp, unsigned short usPort,
        int nKeepIdle = 0, int nKeepInterval = 0, int nKeepCount = 0,
        bool bCloseTcpNagle = false);
        
        bool SendData(SOCKET_FD fd, const char *pBuf, int nSize);
        
        SOCKET_FD Connect(const char *pIp, unsigned short usPort);
        
        void CloseSocketFD(SOCKET_FD fd);
        
        void RunTcpSrv();
        
        bool PushTaskToBack(CTask *pTask);
        bool PushTaskToFront(CTask *pTask);
        
    private:
        static bool OnTcpSrvBaseCallback(TCP_SRV_BASE_MSG msg, SOCKET_FD fd, const char * pData, int nSize);
        static bool OnTcpSrvTaskCallback(CTask *pTask);
        
        
        virtual bool ProcAcceptFD(SOCKET_FD fd, const char * pData, int nSize) = 0;
        virtual bool ProcConnectFD(SOCKET_FD fd) = 0;
        virtual bool ProcDisableFD(SOCKET_FD fd) = 0;
        virtual bool ProcPack(SOCKET_FD fd, const char * pData, int nSize) = 0;
        
        
        bool ProcNeedCut(SOCKET_FD fd);
        bool ProcCut(SOCKET_FD fd, const char * pData, int nSize);
        bool CutPack(SOCKET_FD fd, CBinaryBuf *pBinBuf, const char *pData, int nSize);
        bool CutPack(SOCKET_FD fd, const char *pData, int nSize);
        bool CutUnComplatePack(SOCKET_FD fd, CBinaryBuf *pBinBuf, const char *pData, int nSize);
        bool CutComplatePack(SOCKET_FD fd, CBinaryBuf *pBinBuf, const char *pData, int nSize);
        
        bool AllowHalfPack(SOCKET_FD fd);
        void ClearHalfPack(SOCKET_FD fd);
        CBinaryBuf *PopHalfPack(SOCKET_FD fd);
        bool SetHalfPack (SOCKET_FD fd, CBinaryBuf *pBuf);
        
        
        
    private:
        CTcpSrvBase  *m_pTcpSrvBase;
        CThreadPool  *m_pThreadPool;
        CTcpSrvTaskManager *m_pTaskManager;
        
        std::map<SOCKET_FD, CBinaryBuf *> m_mapHalfPack;
        CShareRWLock m_halfPackLock;
        
        CBinaryMemPool *m_pBinaryMemPool;
        
        bool m_bInit;
        
        static CTcpSrv *m_pThis;
    };
}

#endif //TCP_SRV_HPP