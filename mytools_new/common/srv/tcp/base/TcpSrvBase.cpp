#include "TcpSrvBase.hpp"

#include "../../../file_opt/Log.hpp"
#include <signal.h>
namespace zyb
{
    //CTcpSrvBase::CTcpSrvBase():
    IMPLEMENT_SINGLETON(CTcpSrvBase):
    m_recvNodePool(1000),
    m_recvCache(&CTcpSrvBase::OnDestoryRecvNode, this),
    m_sendCache(0, 0),
    m_closeCache(0, 0)        
    {
        m_pBinaryMemPool = 0;
        m_pDepot         = 0;
        m_pEpoll         = 0;
        m_bInit          = false;
        m_pFun           = 0;
        
        m_tRecvHandle    = 0;
        m_tRecvCond      = PTHREAD_COND_INITIALIZER;
        
        m_tSendHandle    = 0;
        m_tSendCond      = PTHREAD_COND_INITIALIZER;
        
        m_tCloseHandle   = 0;
        m_tCloseCond     = PTHREAD_COND_INITIALIZER;
        
        m_mapWaitSendData.clear();
        
    }
    
    CTcpSrvBase::~CTcpSrvBase()
    {
        m_bInit = false;
        void *pExit = 0;
        if (m_tRecvHandle)
        {
            m_RecvLock.Lock();
            pthread_cond_signal(&m_tRecvCond);
            m_RecvLock.UnLock();
            pthread_join(m_tRecvHandle, &pExit);
            pthread_cond_destroy(&m_tRecvCond);
            m_tRecvCond = PTHREAD_COND_INITIALIZER;
        }
        if (m_tSendHandle)
        {
            m_SendLock.Lock();
            pthread_cond_signal(&m_tSendCond);
            m_SendLock.UnLock();
            pthread_join(m_tSendHandle, &pExit);
            pthread_cond_destroy(&m_tSendCond);
            m_tSendCond = PTHREAD_COND_INITIALIZER;
        }
        if (m_tCloseHandle)
        {
            m_CloseLock.Lock();
            pthread_cond_signal(&m_tCloseCond);
            m_CloseLock.UnLock();
            pthread_join(m_tCloseHandle, &pExit);
            pthread_cond_destroy(&m_tCloseCond);
            m_tCloseCond = PTHREAD_COND_INITIALIZER;
        }
        if (m_pDepot)
        {
            CDepot::ReleaseInstance();
            m_pDepot = 0;
        }
        if (m_pEpoll)
        {
            CTcpEpoll::ReleaseInstance();
            m_pEpoll = 0;
        }
        if (m_pBinaryMemPool)
        {
            CBinaryMemPool::ReleaseInstance();
            m_pBinaryMemPool = 0;
        }
        m_pFun = 0;
    }
    
    bool CTcpSrvBase::InitTcpSrv(const char *pIp, unsigned short usPort, TcpSrvBaseCallbackFun pFun,
        int nKeepIdle/* = 0*/, int nKeepInterval/* = 0*/, int nKeepCount/* = 0*/,
        bool bCloseTcpNagle/* = false*/)
    {
        bool bRet = false;
        for (;;)
        {
            if (m_bInit)
            {
                LOG_ERROR("发生错误， 重复初始化");
                bRet = true;
                break;
            }
            if (!pFun)
            {
                LOG_ERROR("回掉函数 为NULL");
                break;
            }
            m_pFun = pFun;
            
            // 防止链接被关闭后继续写入数据而造成的信号中断程序
            signal(SIGPIPE, SIG_IGN);
            sigset_t signal_mask;
            sigemptyset(&signal_mask);
            sigaddset(&signal_mask, SIGPIPE);
            int rc = pthread_sigmask(SIG_BLOCK, &signal_mask, NULL);
            if (rc != 0)
            {
                LOG_ERROR("SIG_BLOCK错误");
                break;
            }
            
            m_pBinaryMemPool = CBinaryMemPool::GetInstance();
            if (!m_pBinaryMemPool)
            {
                LOG_ERROR("初始化 m_pBinaryMemPool error");
                break;
            }
            m_pDepot = CDepot::GetInstance(m_pBinaryMemPool, &CTcpSrvBase::OnTcpDepotCallbackFun);//new CTcpDepot(m_pBinaryMemPool, &CTcpSrvBase::OnTcpDepotCallbackFun); 
            if (!m_pDepot)
            {
                LOG_ERROR("m_pDepot error");
                break;
            }
            
            
            m_pEpoll = CTcpEpoll::GetInstance();
            if (!m_pEpoll)
            {
                LOG_ERROR("get epoll instakce error");
                break;
            }
            if (!m_pEpoll ->InitTcpEpoll(pIp, usPort, &CTcpSrvBase::OnEpollMsgCallbackFun, nKeepIdle,
                                      nKeepInterval, nKeepCount, bCloseTcpNagle))
            {
                LOG_ERROR("m_pEpoll InitEpoll error");
                break;
            }
            
            m_bInit = true;
            
            // create recv thread
            if (pthread_create(&m_tRecvHandle, 0, &CTcpSrvBase::RecvThread, this))
            {
                m_bInit = false;
                break;
            }
            // create send thread
            if (pthread_create(&m_tSendHandle, 0, &CTcpSrvBase::SendThread, this))
            {
                m_bInit = false;
                break;
            }
            // create close thread
            if (pthread_create(&m_tCloseHandle, 0, &CTcpSrvBase::CloseThread, this))
            {
                m_bInit = false;
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    SOCKET_FD CTcpSrvBase::Connect(const char *pIp, unsigned short usPort)
    {
        SOCKET_FD fd = INVALID_SOCKET_FD;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            
            fd = m_pEpoll ->Connect(pIp, usPort);
            break;
        }
        return fd;
    }
    
    void CTcpSrvBase::TcpSrvBaseStartRun()
    {
        if (!m_bInit)
        {
            LOG_ERROR("don't init");
            return;
        }
        m_pEpoll ->StartRun();
    }
    
    void CTcpSrvBase::CloseSocketFd(SOCKET_FD fd)
    {
        PushCloseSocketFd(fd);
    }
    
    CBinaryMemPool *CTcpSrvBase::GetBinaryMemPool()
    {
        CBinaryMemPool *pRet = 0;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            pRet = m_pBinaryMemPool;
            break;
        }
        return pRet;
    }
    
    bool CTcpSrvBase::SendData(SOCKET_FD fd, const char *pBuf, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            if (!pBuf || 0 >= nSize)
            {
                LOG_ERROR("error pBuf=%x size=%d", pBuf, nSize);
                break;
            }
            if (INVALID_SOCKET_FD == fd)
            {
                LOG_ERROR("error SOCKET_FD");
                break;
            }
            
            CBinaryBuf *pBinBuf = m_pBinaryMemPool ->ProvideBinaryBuf(nSize);
            if (!pBinBuf)
            {
                LOG_ERROR("error pBinBuf is null");
                break;
            }
            
            if (!pBinBuf ->CopyBuf(pBuf, nSize))
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                LOG_ERROR("CopyBuf error");
                break;
            }
            if (!m_pDepot ->PushSendDepot(fd, pBinBuf))
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CTcpSrvBase::CutPack(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            
            bool bOk = true;
            const char *pBuf = 0;
            unsigned int nBufSize = 0;
            CBinaryBuf *pBinBuf = m_pDepot ->PopRecvDepot(fd);
            while (pBinBuf)
            {
                pBuf = pBinBuf ->GetBinaryBuf(nBufSize);
                if (!pBuf || 0 == nBufSize)
                {
                    m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                    bOk = false;
                    break;
                }
                if (!m_pFun(enumTSBM_CUT, fd, pBuf, nBufSize))
                {
                    m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                    bOk = false;
                    break;
                }
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinBuf);
                
                pBinBuf = m_pDepot ->PopRecvDepot(fd);
            }
            
            bRet = bOk;
            break;
        }
        return bRet;
    }
    
    bool CTcpSrvBase::ProcEpollMsgAccept(SOCKET_FD fd, const char *pBuf, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            if (!m_pDepot ->AllotDataDepot(fd))
            {
                // close fd
                PushCloseSocketFd(fd);
                break;
            }
            // callback
            m_pFun(enumTSBM_ACCEPT, fd, pBuf, nSize);
            break;
        }
        return bRet;
    }
    bool CTcpSrvBase::ProcEpollMsgConnect(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            if (!m_pDepot ->AllotDataDepot(fd))
            {
                // close fd
                PushCloseSocketFd(fd);
                break;
            }
            // callback
            m_pFun(enumTSBM_CONNECT, fd, 0, 0);
            bRet = true;
            break;
        }
        return bRet;
    }
    bool CTcpSrvBase::ProcEpollMsgDisconnect(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            PushCloseSocketFd(fd);
            bRet = true;
            break;
        }
        return bRet;
    }
    bool CTcpSrvBase::ProcEpollMsgRecv(SOCKET_FD fd, const char *pBuf, int nBufSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            _tagRecvNode *pNode = m_recvNodePool.ProvideData();
            if (!pNode)
            {
                // close socket
                PushCloseSocketFd(fd);
                LOG_ERROR("allow recv node failed");
                break;
            }
            pNode ->InitRecvNode();
            CBinaryBuf *pBinaryBuf = m_pBinaryMemPool ->ProvideBinaryBuf(nBufSize);
            if (!pBinaryBuf ->CopyBuf(pBuf, (unsigned int)nBufSize))
            {
                m_recvNodePool.DestroyData(pNode);
                // close socket
                PushCloseSocketFd(fd);
                LOG_ERROR("CopyBuf错误 %d", nBufSize);
                break;
            }
            
            pNode ->fd = fd;
            pNode ->pBuf = pBinaryBuf;
            if (!PushRecvData(pNode))
            {
                m_pBinaryMemPool ->DestroyBinaryBuf(pBinaryBuf);
                m_recvNodePool.DestroyData(pNode);
                // close socket
                PushCloseSocketFd(fd);
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CTcpSrvBase::ProcEpollMsgSend(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                LOG_ERROR("don't init");
                break;
            }
            if (!PushSendSocketFd(fd))
            {
                PushCloseSocketFd(fd);
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CTcpSrvBase::PushRecvData(CTcpSrvBase::_tagRecvNode *pNode)
    {
        bool bRet = false;
        for (;;)
        {
            PING_PONG_CODE code = m_recvCache.Push(pNode);
            if (enumPPC_ERROR == code)
            {
                LOG_ERROR("PushRecvData %x", pNode);
                break;
            }
            if (enumPPC_ET == code)
            {
                CAutoLock _lock1(&m_RecvLock);
                pthread_cond_signal(&m_tRecvCond);
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    CTcpSrvBase::_tagRecvNode *CTcpSrvBase::PopRecvData()
    {
        return m_recvCache.Pop(0);
    }
    
    bool CTcpSrvBase::PushSendSocketFd(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            PING_PONG_CODE code = m_sendCache.Push(fd);
            if (enumPPC_ERROR == code)
            {
                LOG_ERROR("PushSendSocketFd %d", fd);
                break;
            }
            if (enumPPC_ET == code)
            {
                CAutoLock _lock1(&m_SendLock);
                pthread_cond_signal(&m_tSendCond);
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    SOCKET_FD CTcpSrvBase::PopSendSocketFd()
    {
        return m_sendCache.Pop(INVALID_SOCKET_FD);
    }
        
    bool CTcpSrvBase::PushCloseSocketFd(SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            PING_PONG_CODE code = m_closeCache.Push(fd);
            if (enumPPC_ERROR == code)
            {
                LOG_ERROR("PushCloseSocketFd %d", fd);
                break;
            }
            if (enumPPC_ET == code)
            {
                CAutoLock _lock1(&m_CloseLock);
                pthread_cond_signal(&m_tCloseCond);
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    SOCKET_FD CTcpSrvBase::PopCloseSocketFd()
    {
        return m_closeCache.Pop(INVALID_SOCKET_FD);
    }
 
    void *CTcpSrvBase::RecvThread(void *pAram)
    {
        CTcpSrvBase* pThis = (CTcpSrvBase *)pAram;
        while (pThis ->m_bInit)
        {
            _tagRecvNode *pNode = 0;
            if (1)
            {
                CAutoLock _lock(&pThis ->m_RecvLock);
                pNode = pThis ->PopRecvData();
                if (!pNode)
                {
                    pthread_cond_wait(&pThis ->m_tRecvCond, pThis ->m_RecvLock.GetMutex());
                    continue;
                }
            }
            if (!pThis ->m_pDepot ->PushRecvDepot(pNode ->fd, pNode ->pBuf))
            {
                pThis ->m_pBinaryMemPool ->DestroyBinaryBuf(pNode ->pBuf);
                pThis ->PushCloseSocketFd(pNode ->fd);
            }
            pNode ->InitRecvNode();
            pThis ->m_recvNodePool.DestroyData(pNode);
        }
        return 0;
    }
    void *CTcpSrvBase::SendThread(void *pAram)
    {
        CTcpSrvBase* pThis = (CTcpSrvBase *)pAram;
        while (pThis ->m_bInit)
        {
            SOCKET_FD fdTemp = INVALID_SOCKET_FD;
            if (1)
            {
                CAutoLock _lock(&pThis ->m_SendLock);
                fdTemp = pThis ->PopSendSocketFd();
                if (INVALID_SOCKET_FD == fdTemp)
                {
                    pthread_cond_wait(&pThis ->m_tSendCond, pThis ->m_SendLock.GetMutex());
                    continue;
                }
            }
            
            // send data
            bool bLocalFind = false;
            CBinaryBuf *pBuf = 0;
            pThis ->m_lockWaitSendData.Lock();
            std::map<SOCKET_FD, CBinaryBuf *>::iterator mapSendIt = pThis ->m_mapWaitSendData.find(fdTemp);
            if (mapSendIt != pThis ->m_mapWaitSendData.end())
            {
                bLocalFind = true;
                pBuf = mapSendIt ->second;
                pThis ->m_lockWaitSendData.UnLock();
            }
            else
            {
                pThis ->m_lockWaitSendData.UnLock();
                pBuf = pThis ->m_pDepot ->PopSendDepot(fdTemp);
            }
            if (!pBuf)
            {
                continue;
            }
            
            unsigned int nValidSize = 0;
            const char *pBufTemp = 0;
            int nSendSize = 0;
            
            bool bSendOK = false;
            while (pBuf)
            {
                pBufTemp = pBuf ->GetBinaryBuf(nValidSize);
                if (!pBufTemp || 0 == nValidSize)
                {
                    break;
                }
                nSendSize = pThis ->m_pEpoll ->SendDataFromFd(fdTemp, pBufTemp, nValidSize);
                if (-1 == nSendSize)
                {
                    break;
                }
                if (nSendSize == nValidSize)
                {
                    pThis ->m_pBinaryMemPool ->DestroyBinaryBuf(pBuf);
                    if (bLocalFind)
                    {
                        bLocalFind = false;
                        pThis ->m_lockWaitSendData.Lock();
                        pThis ->m_mapWaitSendData.erase(fdTemp);
                        pThis ->m_lockWaitSendData.UnLock();
                    }
                    pBuf = pThis ->m_pDepot ->PopSendDepot(fdTemp);
                    if (pBuf)
                    {
                        continue;
                    }
                }
                else
                {
                    if (!pBuf ->Seek(nSendSize))
                    {
                        break;
                    }
                    else
                    {
                        if (!bLocalFind)
                        {
                            pThis ->m_lockWaitSendData.Lock();
                            pThis ->m_mapWaitSendData.insert(std::pair<SOCKET_FD, CBinaryBuf *>(fdTemp, pBuf));
                            pThis ->m_lockWaitSendData.UnLock();
                        }
                    }
                }
                bSendOK = true;
                break;
            }
            if (!bSendOK)
            {
                pThis ->PushCloseSocketFd(fdTemp);
                if (!bLocalFind)
                {
                    pThis ->m_pBinaryMemPool ->DestroyBinaryBuf(pBuf);
                }
            }
            
        }
        return 0;
    }
    void *CTcpSrvBase::CloseThread(void *pAram)
    {
        CTcpSrvBase* pThis = (CTcpSrvBase *)pAram;
        while (pThis ->m_bInit)
        {
            SOCKET_FD fdTemp = INVALID_SOCKET_FD;
            if (1)
            {
                CAutoLock _lock(&pThis ->m_CloseLock);
                fdTemp = pThis ->PopCloseSocketFd();
                if (INVALID_SOCKET_FD == fdTemp)
                {
                    pthread_cond_wait(&pThis ->m_tCloseCond, pThis ->m_CloseLock.GetMutex());
                    continue;
                }
            }
            
            // close
            pThis ->m_pEpoll ->CloseSocketFd(fdTemp);
            pThis ->m_pDepot ->RecoveryDataDepot(fdTemp);
            pThis ->m_lockWaitSendData.Lock();
            pThis ->m_mapWaitSendData.erase(fdTemp);
            pThis ->m_lockWaitSendData.UnLock();
            
            // call back
            pThis ->m_pFun(enumTSBM_DISCONNECT, fdTemp, 0, 0);
        }
        return 0;
    }
        
    bool CTcpSrvBase::OnTcpDepotCallbackFun(DEPOT_MSG msg, SOCKET_FD fd)
    {
        bool bRet = false;
        for (;;)
        {
            CTcpSrvBase *pThis = CTcpSrvBase::GetInstance();
            switch (msg)
            {
                case enumDTM_RECV_PROC:
                {
                    bRet = pThis ->m_pFun(enumTSBM_NEED_CUT, fd, 0, 0);
                    break;
                }
                case enumDTM_SEND_PROC:
                {
                    bRet = pThis ->ProcEpollMsgSend(fd);
                    break;
                }
                default:
                {
                    LOG_ERROR("unknow depot msg=%d", msg);
                    break;
                }
            }
            break;
        }
        return bRet;
    }
    bool CTcpSrvBase::OnEpollMsgCallbackFun(EPOLL_MSG msg, SOCKET_FD fd, const char *pBuf, int nBufSize)
    {
        bool bRet = false;
        for (;;)
        {
            CTcpSrvBase *pThis = CTcpSrvBase::GetInstance();
            switch (msg)
            {
                case enumTCP_EM_ACCEPT:
                {
                    bRet = pThis ->ProcEpollMsgAccept(fd, pBuf, nBufSize);
                    break;
                }
                case enumTCP_EM_CONNECT:
                {
                    bRet = pThis ->ProcEpollMsgConnect(fd);
                    break;
                }
                case enumTCP_EM_DISCONNECT:
                {
                    bRet = pThis ->ProcEpollMsgDisconnect(fd);
                    break;
                }
                case enumTCP_EM_RECV:
                {
                    bRet = pThis ->ProcEpollMsgRecv(fd, pBuf, nBufSize);
                    break;
                }
                case enumTCP_EM_SEND:
                {
                    bRet = pThis ->ProcEpollMsgSend(fd);
                    break;
                }
                default:
                {
                    LOG_ERROR("unknow epoll msg=%d", msg);
                    break;
                }
                
            }
            break;
        }
        return bRet;
    }
    
    void CTcpSrvBase::OnDestoryRecvNode(CTcpSrvBase::_tagRecvNode *pNode)
    {
        m_recvNodePool.DestroyData(pNode);
    }
}