#include "TcpSrv.hpp"
#include "base/TcpSrvBase.hpp"
#include "../../thread_pool/ThreadPool.hpp"
#include "tcp_srv_task/TcpSrvTaskManager.hpp"
#include "../../file_opt/Log.hpp"

#include <netinet/in.h>

namespace zyb
{
    CTcpSrv *CTcpSrv::m_pThis = 0;
    CTcpSrv::CTcpSrv()
    {
        m_pTcpSrvBase  = 0;
        m_pThreadPool  = 0;
        m_pTaskManager = 0;
        
        m_mapHalfPack.clear();
        m_pBinaryMemPool = 0;
        m_bInit = false;
    }
    CTcpSrv::~CTcpSrv()
    {
        if (m_pTcpSrvBase)
        {
            CTcpSrvBase::ReleaseInstance();
            m_pTcpSrvBase = 0;
        }
        if (m_pThreadPool)
        {
            CThreadPool::ReleaseInstance();
            m_pThreadPool = 0;
        }
        m_pThis = 0;
        m_bInit = false;
    }
    
    bool CTcpSrv::InitTcpSrv(const char *pIp, unsigned short usPort,
        int nKeepIdle/* = 0*/, int nKeepInterval/* = 0*/, int nKeepCount/* = 0*/,
        bool bCloseTcpNagle/* = false*/)
    {
        bool bRet = false;
        for (;;)
        {
            if (m_bInit)
            {
                LOG_ERROR("第二次初始化 CTcpSrv，请检查");
                bRet = true;
                break;
            }
            if (m_pThis)
            {
                LOG_ERROR("实例化多次对象");
                break;
            }
            m_pThis = this;
            m_pTaskManager = new CTcpSrvTaskManager();
            if (!m_pTaskManager)
            {
                LOG_ERROR("new CSrvTaskManager error");
                break;
            }
            
            m_pThreadPool = CThreadPool::GetInstance();
            if (!m_pThreadPool)
            {
                LOG_ERROR("CThreadPool::GetInstance error");
                break;
            }
            if (!m_pThreadPool ->InitThreadPool())
            {
                LOG_ERROR("CThreadPool::InitThreadPool error");
                break;
            }
            
            m_pTcpSrvBase = CTcpSrvBase::GetInstance();
            if (!m_pTcpSrvBase)
            {
                LOG_ERROR("CTcpSrv::GetInstance error");
                break;
            }
            if (!m_pTcpSrvBase ->InitTcpSrv(pIp, usPort, CTcpSrv::OnTcpSrvBaseCallback, nKeepIdle, nKeepInterval, nKeepCount, bCloseTcpNagle))
            {
                LOG_ERROR("CTcpSrv::InitTcpSrv error");
                break;
            }
            m_pBinaryMemPool = m_pTcpSrvBase ->GetBinaryMemPool();
            if (!m_pBinaryMemPool)
            {
                LOG_ERROR("m_pBinaryMemPool error");
                break;
            }
            
            m_bInit = true;
            bRet = true;
            break;
        }
        return bRet;
    }
        
    bool CTcpSrv::SendData(SOCKET_FD fd, const char *pBuf, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            if (!m_pTcpSrvBase ->SendData(fd, pBuf, nSize))
            {
                m_pTcpSrvBase ->CloseSocketFd(fd);
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
        
    SOCKET_FD CTcpSrv::Connect(const char *pIp, unsigned short usPort)
    {
        SOCKET_FD fd = INVALID_SOCKET_FD;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            fd = m_pTcpSrvBase ->Connect(pIp, usPort);
            break;
        }
        return fd;
    }
        
    void CTcpSrv::CloseSocketFD(SOCKET_FD fd)
    {
        if (m_bInit)
        {
            m_pTcpSrvBase ->CloseSocketFd(fd);
        }
    }
        
    void CTcpSrv::RunTcpSrv()
    {
        if (!m_bInit)
        {
            LOG_ERROR("don't init");
            return;
        }
        m_pTcpSrvBase ->TcpSrvBaseStartRun();
    }
        
    bool CTcpSrv::PushTaskToBack(CTask *pTask)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            if (!pTask)
            {
                LOG_ERROR("pTask is NULL");
                break;
            }
            bRet = m_pThreadPool ->PushTaskToBack(pTask);
            break;
        }
        return bRet;
    }
    bool CTcpSrv::PushTaskToFront(CTask *pTask)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            if (!pTask)
            {
                LOG_ERROR("pTask is NULL");
                break;
            }
            
            bRet = m_pThreadPool ->PushTaskToFront(pTask);
            break;
        }
        return bRet;
    }
    
    bool CTcpSrv::OnTcpSrvBaseCallback(TCP_SRV_BASE_MSG msg, SOCKET_FD fd, const char * pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_pThis || !m_pThis ->m_bInit)
            {
                LOG_ERROR("CTcpSrv don't init pthis=%x", m_pThis);
                break;
            }
            
            switch (msg)
            {
                case enumTSBM_ACCEPT:
                {
                    if (!m_pThis ->AllowHalfPack(fd))
                    {
                        break;
                    }
                    bRet = m_pThis ->ProcAcceptFD(fd, pData, nSize);
                    break;
                }
                case enumTSBM_CONNECT:
                {
                    if (!m_pThis ->AllowHalfPack(fd))
                    {
                        break;
                    }
                    bRet = m_pThis ->ProcConnectFD(fd);
                    break;
                }
                case enumTSBM_DISCONNECT:
                {
                    m_pThis ->ClearHalfPack(fd);
                    bRet = m_pThis ->ProcDisableFD(fd);
                    break;
                }
                case enumTSBM_NEED_CUT:
                {
                    bRet = m_pThis ->ProcNeedCut(fd);
                    break;
                }
                case enumTSBM_CUT:
                {
                    bRet = m_pThis ->ProcCut(fd, pData, nSize);
                    break;
                }
                default:
                {
                    LOG_ERROR("出现错误， 没有找到指定的类型 %d", msg);
                    break;
                }
            }
            if (!bRet)
            {
                m_pThis ->CloseSocketFD(fd);
            }
            break;
        }
        return bRet;
    }
    
    bool CTcpSrv::OnTcpSrvTaskCallback(CTask *pTask)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_pThis || !m_pThis ->m_bInit)
            {
                LOG_ERROR("CTcpSrv don't init pthis=%x", m_pThis);
                break;
            }
            
            int nTaskType = pTask ->GetTaskType();
            switch (nTaskType)
            {
                case TASK_TYPE_NEED_CUT:
                {
                    CTcpSrvNeedCutTask *pNeedCutTask = dynamic_cast<CTcpSrvNeedCutTask *>(pTask);
                    if (!pNeedCutTask)
                    {
                        LOG_ERROR("CTask * to CTcpSrvNeedCutTask * failed");
                        break;
                    }
                    SOCKET_FD fd = pNeedCutTask ->GetSocketFD();
                    bRet = m_pThis ->m_pTcpSrvBase ->CutPack(fd);
                    if (!bRet)
                    {
                        m_pThis ->CloseSocketFD(fd);
                    }
                    break;
                }
                case TASK_TYPE_PROC:
                {
                    CTcpSrvProcTask *pProcTask = dynamic_cast<CTcpSrvProcTask *>(pTask);
                    if (!pProcTask)
                    {
                        LOG_ERROR("CTask * to CTcpSrvNeedCutTask * failed");
                        break;
                    }
                    SOCKET_FD fd = pProcTask ->GetSocketFD();
                    CBinaryBuf *pBinBuf = pProcTask ->GetBuf();
                    if (!pBinBuf)
                    {
                        LOG_ERROR("pBinBuf is NULL");
                        break;
                    }
                    unsigned int nBufLen = 0;
                    const char *pBuf = pBinBuf ->GetBinaryBuf(nBufLen);
                    if (!pBuf || 0 == nBufLen)
                    {
                        m_pThis ->m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                        LOG_ERROR("pBinBuf is err nBufLen = %u pBuf=%x", nBufLen, pBuf);
                        break;
                    }
                    bRet =m_pThis ->ProcPack(fd, pBuf, nBufLen);
                    m_pThis ->m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                    break;
                }
                default:
                {
                    break;
                }
            }
            break;
        }
        return bRet;
    }
    
    bool CTcpSrv::ProcNeedCut(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            CTask *pTask = m_pTaskManager ->ProvideTask(TASK_TYPE_NEED_CUT);
            if (!pTask)
            {
                break;
            }
            CTcpSrvNeedCutTask *pNeedCutTask = dynamic_cast<CTcpSrvNeedCutTask *>(pTask);
            if (!pNeedCutTask)
            {
                pNeedCutTask ->FreeTask();
                break;
            }
            pNeedCutTask ->InitSrvNeedCutTask(CTcpSrv::OnTcpSrvTaskCallback, fd);
            if (!m_pThreadPool ->PushTaskToBack(pNeedCutTask))
            {
                pNeedCutTask ->FreeTask();
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CTcpSrv::ProcCut(SOCKET_FD fd, const char * pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            CBinaryBuf *pBinBuf = PopHalfPack(fd);
            if (pBinBuf && pBinBuf ->IsEmpty())
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                break;
            }
            
            bRet = CutPack(fd, pBinBuf, pData, nSize);
            break;
        }
        return bRet;
    }
    
    bool CTcpSrv::CutPack(SOCKET_FD fd, CBinaryBuf *pBinBuf, const char *pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!pBinBuf)
            {
                bRet = CutPack(fd, pData, nSize);
            }
            else
            {
                unsigned int nBufSize = 0;
                const char *pBuf = pBinBuf ->GetBinaryBuf(nBufSize);
                if (!pBuf || 0 == nBufSize)
                {
                    m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                    break;
                }
                
                int nPackLen = 0;
                if (1 == nBufSize)
                {
                    char szLen[2];
                    szLen[0] = pBuf[0];
                    szLen[1] = pData[0];
                    nPackLen = ntohs(*((unsigned short *)szLen));
                }
                else
                {
                    nPackLen = ntohs(*((unsigned short *)pBuf));
                }
                
                if (nPackLen > nBufSize + nSize)
                {
                    bRet = CutUnComplatePack(fd, pBinBuf, pData, nSize);
                }
                else if (nPackLen == nBufSize + nSize)
                {
                    bRet = CutComplatePack(fd, pBinBuf, pData, nSize);
                }
                else
                {
                    int nAllowSize = nPackLen - nBufSize;
                    if (!CutComplatePack(fd, pBinBuf, pData, nAllowSize))
                    {
                        break;
                    }
                    bRet = CutPack(fd, &pData[nAllowSize], nSize - nAllowSize);
                } 
            }
            break;
        }
        return bRet;
    }
    
    bool CTcpSrv::CutPack(SOCKET_FD fd, const char *pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            int nPackLen = 512;
            if (1 < nSize)
            {
                nPackLen = ntohs(*((unsigned short *)pData));
            }
            CBinaryBuf *pBuf = m_pBinaryMemPool ->ProvideBinaryBuf(nPackLen);
            if (!pBuf)
            {
                break;
            }
            if (nPackLen > nSize)
            {
                bRet = CutUnComplatePack(fd, pBuf, pData, nSize);
            }
            else if (nPackLen == nSize)
            {
                bRet = CutComplatePack(fd, pBuf, pData, nSize);
            }
            else
            {
                if (!CutComplatePack(fd, pBuf, pData, nPackLen))
                {
                    break;
                }
                    
                pData = &pData[nPackLen];
                nSize -= nPackLen;
                continue;
            }
            break;
        }
        return bRet;
    }
    
    bool CTcpSrv::CutUnComplatePack(SOCKET_FD fd, CBinaryBuf *pBinBuf, const char *pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!pBinBuf ->AppendBuf(pData, nSize) || !SetHalfPack(fd, pBinBuf))
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    bool CTcpSrv::CutComplatePack(SOCKET_FD fd, CBinaryBuf *pBinBuf, const char *pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!pBinBuf ->AppendBuf(pData, nSize))
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                break;
            }
            CTask *pTask = m_pTaskManager ->ProvideTask(TASK_TYPE_PROC);
            CTcpSrvProcTask *pProcTask = dynamic_cast<CTcpSrvProcTask *>(pTask);
            if (!pProcTask)
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                m_pTaskManager ->RecoveryTask(pTask);
                break;
            }
            pProcTask ->InitSrvProcTask(CTcpSrv::OnTcpSrvTaskCallback, pBinBuf, fd);
            if (!m_pThreadPool ->PushTaskToBack(pTask))
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                m_pTaskManager ->RecoveryTask(pTask);
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CTcpSrv::AllowHalfPack(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            CAutoShareReadLock _autoLock(&m_halfPackLock, CAutoShareReadLock::emLT_ReadLock);
            std::map<SOCKET_FD, CBinaryBuf *>::iterator mapIt = m_mapHalfPack.find(fd);
            if (mapIt != m_mapHalfPack.end())
            {
                LOG_ERROR("AllowHalfPack error fd=%d", fd);
                break;
            }
            std::pair<std::map<SOCKET_FD, CBinaryBuf *>::iterator, bool> ret;
            ret = m_mapHalfPack.insert(std::pair<SOCKET_FD, CBinaryBuf *>(fd, 0));
            bRet = ret.second;
            break;
        }
        return bRet;
    }
    
    void CTcpSrv::ClearHalfPack(SOCKET_FD fd)
    {
        CAutoShareReadLock _autoLock(&m_halfPackLock, CAutoShareReadLock::emLT_WriteLock);
        std::map<SOCKET_FD, CBinaryBuf *>::iterator mapIt = m_mapHalfPack.find(fd);
        if (mapIt != m_mapHalfPack.end())
        {
            if (mapIt ->second)
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(mapIt ->second);
                m_mapHalfPack.erase(mapIt);
            }
        }
    }
    
    CBinaryBuf *CTcpSrv::PopHalfPack(SOCKET_FD fd)
    {
        CBinaryBuf *pBuf = 0;
        for (;;)
        {
            CAutoShareReadLock _autoLock(&m_halfPackLock, CAutoShareReadLock::emLT_ReadLock);
            std::map<SOCKET_FD, CBinaryBuf *>::iterator mapIt = m_mapHalfPack.find(fd);
            if (mapIt == m_mapHalfPack.end())
            {
                break;
            }
            pBuf = mapIt ->second;
            mapIt ->second = 0;
            break;
        }
        return pBuf;
    }
    
    bool CTcpSrv::SetHalfPack (SOCKET_FD fd, CBinaryBuf *pBuf)
    {
        bool bRet = false;
        for (;;)
        {
            if (!pBuf || pBuf ->IsEmpty())
            {
                LOG_ERROR("pBuf error pBuf=%x", pBuf);
                break;
            }
            CAutoShareReadLock _autoLock(&m_halfPackLock, CAutoShareReadLock::emLT_WriteLock);
            std::map<SOCKET_FD, CBinaryBuf *>::iterator mapIt = m_mapHalfPack.find(fd);
            if (mapIt == m_mapHalfPack.end())
            {
                break;
            }
            if (mapIt ->second && mapIt ->second != pBuf)
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(mapIt ->second);
            }
            mapIt ->second = pBuf;
            bRet = true;
            break;
        }
        return bRet;
    }
}