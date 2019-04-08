#include "UdpSrv.hpp"
#include "base/UdpSrvBase.hpp"
#include "../../thread_pool/ThreadPool.hpp"
#include "udp_srv_task/UdpSrvTaskManager.hpp"
#include "../../file_opt/Log.hpp"
#include "udp_srv_task/UdpSrvTask.hpp"

#include <netinet/in.h>

namespace zyb
{
    CUdpSrv *CUdpSrv::m_pThis = 0;
    CUdpSrv::CUdpSrv()
    {
        m_pUdpSrvBase  = 0;
        m_pThreadPool  = 0;
        m_pTaskManager = 0;
        m_pBinaryMemPool = 0;
        m_bInit = false;
    }
    CUdpSrv::~CUdpSrv()
    {
        if (m_pUdpSrvBase)
        {
            CUdpSrvBase::ReleaseInstance();
            m_pUdpSrvBase = 0;
        }
        if (m_pThreadPool)
        {
            CThreadPool::ReleaseInstance();
            m_pThreadPool = 0;
        }
        m_pThis = 0;
        m_bInit = false;
    }
    
    bool CUdpSrv::InitUdpSrv(const char *pIp, unsigned short usPort)
    {
        bool bRet = false;
        for (;;)
        {
            if (m_bInit)
            {
                LOG_ERROR("第二次初始化 CUdpSrv，请检查");
                bRet = true;
                break;
            }
            if (m_pThis)
            {
                LOG_ERROR("实例化多次对象");
                break;
            }
            m_pThis = this;
            m_pTaskManager = new CUdpSrvTaskManager();
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
            
            m_pUdpSrvBase = CUdpSrvBase::GetInstance();
            if (!m_pUdpSrvBase)
            {
                LOG_ERROR("CUdpSrv::GetInstance error");
                break;
            }
            if (!m_pUdpSrvBase ->InitUdpSrv(pIp, usPort, CUdpSrv::OnUdpSrvBaseCallback))
            {
                LOG_ERROR("CUdpSrv::InitTcpSrv error");
                break;
            }
            m_pBinaryMemPool = m_pUdpSrvBase ->GetBinaryMemPool();
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
        
    bool CUdpSrv::SendData(SOCKET_FD fd, const char *pBuf, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            if (!m_pUdpSrvBase ->SendData(fd, pBuf, nSize))
            {
                //m_pUdpSrvBase ->CloseSocketFd(fd);
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
        
    SOCKET_FD CUdpSrv::Connect(const char *pIp, unsigned short usPort)
    {
        SOCKET_FD fd = INVALID_SOCKET_FD;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            fd = m_pUdpSrvBase ->Connect(pIp, usPort);
            break;
        }
        return fd;
    }
        
    void CUdpSrv::CloseSocketFD(SOCKET_FD fd)
    {
        if (m_bInit)
        {
            m_pUdpSrvBase ->CloseSocketFd(fd);
        }
    }
        
    void CUdpSrv::RunTcpSrv()
    {
        if (!m_bInit)
        {
            LOG_ERROR("don't init");
            return;
        }
        m_pUdpSrvBase ->UdpSrvBaseStartRun();
    }
        
    bool CUdpSrv::PushTaskToBack(CTask *pTask)
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
    bool CUdpSrv::PushTaskToFront(CTask *pTask)
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
    
    bool CUdpSrv::OnUdpSrvBaseCallback(UDP_SRV_BASE_MSG msg, SOCKET_FD fd, const char * pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_pThis || !m_pThis ->m_bInit)
            {
                LOG_ERROR("CUdpSrv don't init pthis=%x", m_pThis);
                break;
            }
            
            switch (msg)
            {
                case enumUSBM_ACCEPT:
                {
                    bRet = m_pThis ->ProcAcceptFD(fd);
                    break;
                }
                case enumUSBM_CONNECT:
                {
                    bRet = m_pThis ->ProcConnectFD(fd);
                    break;
                }
                case enumUSBM_DISCONNECT:
                {
                    bRet = m_pThis ->ProcDisableFD(fd);
                    break;
                }
                case enumUSBM_PROC_PACK:
                {
                    bRet = m_pThis ->ProcPack(fd);
                    break;
                }
                case enumUSBM_VALIDATE:
                {
                    bRet = m_pThis ->ProcValidate(fd, pData, nSize);
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
    
    bool CUdpSrv::ProcPack(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            CTask *pTask = m_pTaskManager ->ProvideTask(TASK_TYPE_FD_PROC);
            if (!pTask)
            {
                break;
            }
            CUdpSrvFdProcTask *pFdProcTask = dynamic_cast<CUdpSrvFdProcTask *>(pTask);
            if (!pFdProcTask)
            {
                pFdProcTask ->FreeTask();
                break;
            }
            pFdProcTask ->InitSrvFDProcTask(CUdpSrv::OnUdpSrvTaskCallback, fd);
            if (!m_pThreadPool ->PushTaskToBack(pFdProcTask))
            {
                pFdProcTask ->FreeTask();
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    bool CUdpSrv::ProcValidate(SOCKET_FD fd, const char * pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CUdpSrv::OnUdpSrvTaskCallback(CTask *pTask)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_pThis || !m_pThis ->m_bInit)
            {
                LOG_ERROR("CUdpSrv don't init pthis=%x", m_pThis);
                break;
            }
            
            int nTaskType = pTask ->GetTaskType();
            switch (nTaskType)
            {
                case TASK_TYPE_FD_PROC:
                {
                    CUdpSrvFdProcTask *pFdProcTask = dynamic_cast<CUdpSrvFdProcTask *>(pTask);
                    if (!pFdProcTask)
                    {
                        LOG_ERROR("CTask * to CUdpSrvFdProcTask * failed");
                        break;
                    }
                    SOCKET_FD fd = pFdProcTask ->GetSocketFD();
                    
                    bRet = true;
                    CBinaryBuf *pBinBuf = m_pThis ->m_pUdpSrvBase ->PopRecvPack(fd);
                    while (pBinBuf)
                    {
                        CTask *pTask = m_pThis ->m_pTaskManager ->ProvideTask(TASK_TYPE_PROC);
                        CUdpSrvProcTask *pProcTask = dynamic_cast<CUdpSrvProcTask *>(pTask);
                        if (!pProcTask)
                        {
                            bRet = false;
                            break;
                        }
                        pProcTask ->InitSrvProcTask(CUdpSrv::OnUdpSrvTaskCallback, pBinBuf, fd);
                        if (!m_pThis ->m_pThreadPool ->PushTaskToBack(pTask))
                        {
                            bRet = false;
                            break;
                        }
                        pBinBuf = m_pThis ->m_pUdpSrvBase ->PopRecvPack(fd);
                    }
                    
                    if (!bRet)
                    {
                        m_pThis ->m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                        m_pThis ->m_pTaskManager ->RecoveryTask(pTask);
                        m_pThis ->CloseSocketFD(fd);
                    }
                    break;
                }
                case TASK_TYPE_PROC:
                {
                    CUdpSrvProcTask *pProcTask = dynamic_cast<CUdpSrvProcTask *>(pTask);
                    if (!pProcTask)
                    {
                        LOG_ERROR("CTask * to CUdpSrvProcTask * failed");
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
   
}