#include "DataDepot.hpp"
#include "../../file_opt/Log.hpp"

namespace zyb
{
    CDepotBase::CDepotBase(DEPOT_TYPE type, CBinaryMemPool *pBinaryMemPool, DepotCallbackFun pFun)
    :cache(&CDepotBase::OnDestoryFun, this)
    {
        m_pBinaryMemPool = pBinaryMemPool;
        m_pFun           = pFun;
        m_depotType      = type;
        if (!m_pBinaryMemPool || !m_pFun || !(enumTDT_RECV == m_depotType || enumTDT_SEND == m_depotType))
        {
            LOG_ERROR("参数错误 m_pBinaryMemPool=%x m_pFun=%x, m_depotType=%d", m_pBinaryMemPool, m_pFun, m_depotType);
        }
        m_fd             = INVALID_SOCKET_FD;
        m_bInit          = false;
        
    }
    
    CDepotBase::~CDepotBase()
    {
        ClearDepot();
    }
    
    bool CDepotBase::InitDepot(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (m_bInit)
            {
                LOG_ERROR("发生错误 重复初始化 newfd=%d oldfd=%d", fd, m_fd);
                break;
            }
            m_fd             = fd;
            m_bInit          = true;
            bRet             = true;
            break;
        }
        return bRet;
    }
    
    bool CDepotBase::ClearDepot()
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("没有初始化");
                break;
            }
            cache.ClearCache();
            m_fd             = INVALID_SOCKET_FD;
            m_bInit          = false;
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CDepotBase::PushData(CBinaryBuf *pBuf)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("没有初始化");
                break;
            }
            PING_PONG_CODE code = cache.Push(pBuf);
            if (enumPPC_ET == code)
            {
                if (enumTDT_RECV == m_depotType)
                {
                    m_pFun(enumDTM_RECV_PROC, m_fd);
                }
                else if (enumTDT_SEND == m_depotType)
                {
                    m_pFun(enumDTM_SEND_PROC, m_fd);
                }
                else
                {
                    LOG_ERROR("m_depotType err %d", m_depotType);
                    break;
                }
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    CBinaryBuf *CDepotBase::PopData()
    {
        CBinaryBuf *pRet = 0;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("没有初始化");
                break;
            }
            pRet = cache.Pop(0);
            break;
        }
        return pRet;
    }
    
    void CDepotBase::OnDestoryFun(CBinaryBuf *pBuf)
    {
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("OnDestoryFun 没有初始化");
                break;
            }
            if (!m_pBinaryMemPool)
            {
                LOG_ERROR("m_pBinaryMemPool is NULL");
                break;
            }
            m_pBinaryMemPool ->DestroyBinaryBuf(pBuf);
            break;
        }
    }
    
    CRecvDepot::CRecvDepot(DEPOT_TYPE type, CBinaryMemPool* pBinaryMemPool, DepotCallbackFun pFun):
    CDepotBase(type, pBinaryMemPool, pFun)
    {
        
    }
    
    CSendDepot::CSendDepot(DEPOT_TYPE type, CBinaryMemPool* pBinaryMemPool, DepotCallbackFun pFun):
    CDepotBase(type, pBinaryMemPool, pFun)
    {
        
    }
    
    IMPLEMENT_SINGLETON_2(CDepot, CBinaryMemPool *, pBinMemPool, DepotCallbackFun, pFun):
    m_memPoolRecvDepot(100, enumTDT_RECV, pBinMemPool, pFun),
    m_memPoolSendDepot(100, enumTDT_SEND, pBinMemPool, pFun)        
    {
        
        
    }
    
    CDepot::~CDepot()
    {
        
    }
    
    bool CDepot::AllotDataDepot(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            if (!AllotRecvDataDepot(fd))
            {
                break;
            }
            if (!AllotSendDataDepot(fd))
            {
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CDepot::RecoveryDataDepot(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            RecoveryRecvDataDepot(fd);
            RecoverySendDataDepot(fd);
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CDepot::PushRecvDepot(SOCKET_FD fd, CBinaryBuf *pBuf)
    {
        bool bRet = false;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            int nIndex = fd % DEPOT_NUM;
            CAutoLock _lock(&m_arrRecvLock[nIndex]);
            
            std::map<SOCKET_FD, CRecvDepot *>::iterator mapIt = m_mapArrRecvDepot[nIndex].find(fd);
            if (mapIt == m_mapArrRecvDepot[nIndex].end())
            {
                break;
            }
            bRet =mapIt ->second ->PushData(pBuf);
            break;
        }
        return bRet;
    }
    
    CBinaryBuf *CDepot::PopRecvDepot(SOCKET_FD fd)
    {
        CBinaryBuf *pRet = 0;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            int nIndex = fd % DEPOT_NUM;
            CAutoLock _lock(&m_arrRecvLock[nIndex]);
            
            std::map<SOCKET_FD, CRecvDepot *>::iterator mapIt = m_mapArrRecvDepot[nIndex].find(fd);
            if (mapIt == m_mapArrRecvDepot[nIndex].end())
            {
                break;
            }
            pRet =mapIt ->second ->PopData();
            break;
        }
        return pRet;
    }
    
    bool CDepot::PushSendDepot(SOCKET_FD fd, CBinaryBuf *pBuf)
    {
        bool bRet = false;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            int nIndex = fd % DEPOT_NUM;
            CAutoLock _lock(&m_arrSendLock[nIndex]);
            
            std::map<SOCKET_FD, CSendDepot *>::iterator mapIt = m_mapArrSendDepot[nIndex].find(fd);
            if (mapIt == m_mapArrSendDepot[nIndex].end())
            {
                break;
            }
            bRet =mapIt ->second ->PushData(pBuf);
            break;
        }
        return bRet;
    }
    
    CBinaryBuf *CDepot::PopSendDepot(SOCKET_FD fd)
    {
        CBinaryBuf *pRet = 0;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            int nIndex = fd % DEPOT_NUM;
            CAutoLock _lock(&m_arrSendLock[nIndex]);
            
            std::map<SOCKET_FD, CSendDepot *>::iterator mapIt = m_mapArrSendDepot[nIndex].find(fd);
            if (mapIt == m_mapArrSendDepot[nIndex].end())
            {
                break;
            }
            pRet =mapIt ->second ->PopData();
            break;
        }
        return pRet;
    }
    
    bool CDepot::AllotRecvDataDepot(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            int nIndex = fd % DEPOT_NUM;
            CAutoLock _lock(&m_arrRecvLock[nIndex]);
            
            std::map<SOCKET_FD, CRecvDepot *>::iterator mapIt = m_mapArrRecvDepot[nIndex].find(fd);
            if (mapIt != m_mapArrRecvDepot[nIndex].end())
            {
                break;
            }
            
            CRecvDepot *pRecvDepot = m_memPoolRecvDepot.ProvideData();
            if (!pRecvDepot)
            {
                break;
            }
            if (!pRecvDepot ->InitDepot(fd))
            {
                break;
            }
            std::pair<std::map<SOCKET_FD, CRecvDepot *>::iterator, bool> ret;
            ret = m_mapArrRecvDepot[nIndex].insert(std::pair<SOCKET_FD, CRecvDepot *>(fd, pRecvDepot));
            if (!ret.second)
            {
                m_memPoolRecvDepot.DestroyData(pRecvDepot);
                break;
            }
            bRet = ret.second;
            break;
        }
        return bRet;
    }
    
    bool CDepot::AllotSendDataDepot(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            int nIndex = fd % DEPOT_NUM;
            CAutoLock _lock(&m_arrSendLock[nIndex]);
            
            std::map<SOCKET_FD, CSendDepot *>::iterator mapIt = m_mapArrSendDepot[nIndex].find(fd);
            if (mapIt != m_mapArrSendDepot[nIndex].end())
            {
                break;
            }
            
            CSendDepot *pSendDepot = m_memPoolSendDepot.ProvideData();
            if (!pSendDepot)
            {
                break;
            }
            if (!pSendDepot ->InitDepot(fd))
            {
                break;
            }
            std::pair<std::map<SOCKET_FD, CSendDepot *>::iterator, bool> ret;
            ret = m_mapArrSendDepot[nIndex].insert(std::pair<SOCKET_FD, CSendDepot *>(fd, pSendDepot));
            if (!ret.second)
            {
                m_memPoolSendDepot.DestroyData(pSendDepot);
                break;
            }
            bRet = ret.second;
            break;
        }
        return bRet;
    }
    
    bool CDepot::RecoveryRecvDataDepot(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            CRecvDepot *pRecvDepot = 0;
            if (1)
            {
                int nIndex = fd % DEPOT_NUM;
                CAutoLock _lock(&m_arrRecvLock[nIndex]);
            
                std::map<SOCKET_FD, CRecvDepot *>::iterator mapIt = m_mapArrRecvDepot[nIndex].find(fd);
                if (mapIt == m_mapArrRecvDepot[nIndex].end())
                {
                    break;
                }
                pRecvDepot = mapIt ->second;
                m_mapArrRecvDepot[nIndex].erase(mapIt);
            }

            if (pRecvDepot)
            {
                pRecvDepot ->ClearDepot();
                m_memPoolRecvDepot.DestroyData(pRecvDepot);
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CDepot::RecoverySendDataDepot(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error fd");
                break;
            }
            CSendDepot *pSendDepot = 0;
            if (1)
            {
                int nIndex = fd % DEPOT_NUM;
                CAutoLock _lock(&m_arrSendLock[nIndex]);
            
                std::map<SOCKET_FD, CSendDepot *>::iterator mapIt = m_mapArrSendDepot[nIndex].find(fd);
                if (mapIt == m_mapArrSendDepot[nIndex].end())
                {
                    break;
                }
                pSendDepot = mapIt ->second;
                m_mapArrSendDepot[nIndex].erase(mapIt);
            }

            if (pSendDepot)
            {
                pSendDepot ->ClearDepot();
                m_memPoolSendDepot.DestroyData(pSendDepot);
            }
            bRet = true;
            break;
        }
        return bRet;
    }
}