#ifndef TCP_SRV_BASE_HPP
#define TCP_SRV_BASE_HPP

#include "../../../buffer/BinaryMemPool.hpp"
#include "../../../epoll/TcpEopll.hpp"
#include "../../data_depot/DataDepot.hpp"
#include "../../../buffer/PingPongCache.hpp"
#include "TcpSrvBaseMsg.hpp"

namespace zyb
{
    typedef bool (*TcpSrvBaseCallbackFun)(TCP_SRV_BASE_MSG, SOCKET_FD, const char *, int);
    
    class CTcpSrvBase
    {
        struct _tagRecvNode
        {
            SOCKET_FD fd;
            CBinaryBuf *pBuf;
            void InitRecvNode()
            {
                fd = INVALID_SOCKET_FD;
                pBuf = 0;
            }
        };
        friend class CPingPongCache<CTcpSrvBase, CTcpSrvBase::_tagRecvNode *>;
        DECLARE_SINGLETON(CTcpSrvBase)
    public:
        ~CTcpSrvBase();
        
        bool InitTcpSrv(const char *pIp, unsigned short usPort, TcpSrvBaseCallbackFun pFun,
        int nKeepIdle = 0, int nKeepInterval = 0, int nKeepCount = 0,
        bool bCloseTcpNagle = false);
        
        SOCKET_FD Connect(const char *pIp, unsigned short usPort);
        
        void TcpSrvBaseStartRun();
        void CloseSocketFd(SOCKET_FD fd);
        
        CBinaryMemPool *GetBinaryMemPool();
        bool SendData(SOCKET_FD fd, const char *pBuf, int nSize);
        
        bool CutPack(SOCKET_FD fd);
    private:
        bool ProcEpollMsgAccept(SOCKET_FD fd, const char *pBuf, int nSize);
        bool ProcEpollMsgConnect(SOCKET_FD fd);
        bool ProcEpollMsgDisconnect(SOCKET_FD fd);
        bool ProcEpollMsgRecv(SOCKET_FD fd, const char *pBuf, int nBufSize);
        bool ProcEpollMsgSend(SOCKET_FD fd);
        
        bool PushRecvData(CTcpSrvBase::_tagRecvNode *pNode);
        CTcpSrvBase::_tagRecvNode *PopRecvData();
        
        bool PushSendSocketFd(SOCKET_FD fd);
        SOCKET_FD PopSendSocketFd();
        
        bool PushCloseSocketFd(SOCKET_FD fd);
        SOCKET_FD PopCloseSocketFd();
        
        static void *RecvThread(void *pAram);
        static void *SendThread(void *pAram);
        static void *CloseThread(void *pAram);
        
        static bool OnTcpDepotCallbackFun(DEPOT_MSG msg, SOCKET_FD fd);
        static bool OnEpollMsgCallbackFun(EPOLL_MSG msg, SOCKET_FD fd, const char *pBuf, int nBufSize);
        
        void OnDestoryRecvNode(CTcpSrvBase::_tagRecvNode *pNode);
        
    private:
        CBinaryMemPool *m_pBinaryMemPool;
        CDepot *m_pDepot;
        CTcpEpoll *m_pEpoll;
        bool m_bInit;
        TcpSrvBaseCallbackFun m_pFun;
        
        //recv
        pthread_t m_tRecvHandle;
        pthread_cond_t m_tRecvCond;
        CLock m_RecvLock;
        
        //send
        pthread_t m_tSendHandle;
        pthread_cond_t m_tSendCond;
        CLock m_SendLock;
        
        //close
        pthread_t m_tCloseHandle;
        pthread_cond_t m_tCloseCond;
        CLock m_CloseLock;
        
        CPingPongCache<CTcpSrvBase, CTcpSrvBase::_tagRecvNode *>m_recvCache;
        CMemoryPool0<_tagRecvNode> m_recvNodePool;
        
        CPingPongCache<CTcpSrvBase, SOCKET_FD>m_sendCache;
        CPingPongCache<CTcpSrvBase, SOCKET_FD>m_closeCache;
        
        CLock m_lockWaitSendData;
        std::map<SOCKET_FD, CBinaryBuf *> m_mapWaitSendData;
    };
}

#endif // TCP_SRV_BASE_HPP