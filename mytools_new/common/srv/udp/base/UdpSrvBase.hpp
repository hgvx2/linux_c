#ifndef UDP_SRV_BASE_HPP
#define UDP_SRV_BASE_HPP
#include "../../../buffer/BinaryMemPool.hpp"
#include "../../../epoll/UdpEpoll.hpp"
#include "../../data_depot/DataDepot.hpp"
#include "../../../buffer/PingPongCache.hpp"
#include "UdpSrvBaseMsg.hpp"

namespace zyb
{
    typedef bool (*UdpSrvBaseCallbackFun)(UDP_SRV_BASE_MSG, SOCKET_FD, const char *, int);
    
    class CUdpSrvBase
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
        friend class CPingPongCache<CUdpSrvBase, CUdpSrvBase::_tagRecvNode *>;
        DECLARE_SINGLETON(CUdpSrvBase)
                
    public:
        ~CUdpSrvBase();
        
        bool InitUdpSrv(const char *pIp, unsigned short usPort, UdpSrvBaseCallbackFun pFun);
        
        SOCKET_FD Connect(const char *pIp, unsigned short usPort);
        
        void UdpSrvBaseStartRun();
        void CloseSocketFd(SOCKET_FD fd);
        
        CBinaryMemPool *GetBinaryMemPool();
        bool SendData(SOCKET_FD fd, const char *pBuf, int nSize);
        
        CBinaryBuf * PopRecvPack(SOCKET_FD fd);
        
    private:
        
        bool ProcEpollMsgAccept(SOCKET_FD fd);
        bool ProcEpollMsgConnect(SOCKET_FD fd);
        bool ProcEpollMsgDisconnect(SOCKET_FD fd);
        bool ProcEpollMsgRecv(SOCKET_FD fd, const char *pBuf, int nBufSize);
        bool ProcEpollMsgSend(SOCKET_FD fd);
        
        bool PushCloseSocketFd(SOCKET_FD fd);
        SOCKET_FD PopCloseSocketFd();
        
        bool PushSendSocketFd(SOCKET_FD fd);
        SOCKET_FD PopSendSocketFd();
        
        bool PushRecvData(CUdpSrvBase::_tagRecvNode *pNode);
        CUdpSrvBase::_tagRecvNode *PopRecvData();
        
        static void *RecvThread(void *pAram);
        static void *SendThread(void *pAram);
        static void *CloseThread(void *pAram);
        
        static bool OnDepotCallbackFun(DEPOT_MSG msg, SOCKET_FD fd);
        static bool OnEpollMsgCallbackFun(EPOLL_MSG msg, SOCKET_FD fd, const char *pBuf, int nBufSize);
        
        void OnDestoryRecvNode(CUdpSrvBase::_tagRecvNode *pNode);
        
    private:
        CBinaryMemPool *m_pBinaryMemPool;
        CDepot *m_pDepot;
        CUdpEpoll *m_pEpoll;
        bool m_bInit;
        UdpSrvBaseCallbackFun m_pFun;
        
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
        
        CPingPongCache<CUdpSrvBase, CUdpSrvBase::_tagRecvNode *>m_recvCache;
        CMemoryPool0<_tagRecvNode> m_recvNodePool;
        
        CPingPongCache<CUdpSrvBase, SOCKET_FD>m_sendCache;
        CPingPongCache<CUdpSrvBase, SOCKET_FD>m_closeCache;
        
    };
}

#endif // UDP_SRV_BASE_HPP