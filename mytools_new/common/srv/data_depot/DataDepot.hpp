#ifndef DATA_DEPOT_HPP
#define DATA_DEPOT_HPP

#include "../../buffer/BinaryMemPool.hpp"
#include "../../epoll/SocketDefault.hpp"
#include "../../buffer/PingPongCache.hpp"
#include "../../design_patterns/Singleton.hpp"

namespace zyb
{
    #define DEPOT_NUM      (20)

    typedef enum _DEPOT_MSG
    {
        enumDTM_RECV_PROC  = 1,
        enumDTM_SEND_PROC  = 2
    }DEPOT_MSG;
    
    typedef enum _DEPOT_TYPE
    {
        enumTDT_UNKNOW = 0,
        enumTDT_RECV = 1,
        enumTDT_SEND = 2
    }DEPOT_TYPE;
    
    typedef bool (*DepotCallbackFun)(DEPOT_MSG, SOCKET_FD);
    class CDepotBase
    {
        friend class CRecvDepot;
        friend class CSendDepot;
        friend class CPingPongCache<CDepotBase, CBinaryBuf *>;
        
        CDepotBase(DEPOT_TYPE type, CBinaryMemPool *pBinaryMemPool, DepotCallbackFun pFun);
    public:
        ~CDepotBase();
        
        bool InitDepot(SOCKET_FD fd);
        bool ClearDepot();
        
        bool PushData(CBinaryBuf *pBuf);
        CBinaryBuf *PopData();
        
    private:
        void OnDestoryFun(CBinaryBuf *pBuf);
    private:
        CPingPongCache<CDepotBase, CBinaryBuf *> cache;
        CBinaryMemPool *m_pBinaryMemPool;
        DepotCallbackFun m_pFun;
        SOCKET_FD m_fd;
        bool m_bInit;
        CLock m_lock;
        DEPOT_TYPE m_depotType;
    };
    
    class CRecvDepot : public CDepotBase
    {
        friend class CMemoryPool3<CRecvDepot, DEPOT_TYPE, CBinaryMemPool *, DepotCallbackFun>;
        CRecvDepot(DEPOT_TYPE type, CBinaryMemPool *pBinaryMemPool, DepotCallbackFun pFun);
    };
    
    class CSendDepot : public CDepotBase
    {
        friend class CMemoryPool3<CSendDepot, DEPOT_TYPE, CBinaryMemPool *, DepotCallbackFun>;
        CSendDepot(DEPOT_TYPE type, CBinaryMemPool *pBinaryMemPool, DepotCallbackFun pFun);
    };
    
    class CDepot
    {
        DECLARE_SINGLETON_2(CDepot, CBinaryMemPool *, pBinMemPool, DepotCallbackFun, pFun)
    public:
        ~CDepot();
        
        bool AllotDataDepot(SOCKET_FD fd);
        bool RecoveryDataDepot(SOCKET_FD fd);
        
        bool PushRecvDepot(SOCKET_FD fd, CBinaryBuf *pBuf);
        CBinaryBuf *PopRecvDepot(SOCKET_FD fd);
        bool PushSendDepot(SOCKET_FD fd, CBinaryBuf *pBuf);
        CBinaryBuf *PopSendDepot(SOCKET_FD fd);
        
    private:
        bool AllotRecvDataDepot(SOCKET_FD fd);
        bool AllotSendDataDepot(SOCKET_FD fd);
        bool RecoveryRecvDataDepot(SOCKET_FD fd);
        bool RecoverySendDataDepot(SOCKET_FD fd);
         
    private:
        CMemoryPool3<CRecvDepot, DEPOT_TYPE, CBinaryMemPool *, DepotCallbackFun> m_memPoolRecvDepot;
        CMemoryPool3<CSendDepot, DEPOT_TYPE, CBinaryMemPool *, DepotCallbackFun> m_memPoolSendDepot;
       
        CLock m_arrRecvLock[DEPOT_NUM];
        std::map<SOCKET_FD, CRecvDepot *>m_mapArrRecvDepot[DEPOT_NUM];
        CLock m_arrSendLock[DEPOT_NUM];
        std::map<SOCKET_FD, CSendDepot *>m_mapArrSendDepot[DEPOT_NUM];
    };
}

#endif // DATA_DEPOT_HPP