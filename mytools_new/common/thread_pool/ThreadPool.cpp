#include "ThreadPool.hpp"
#include "TaskQueue.hpp"
#include "../file_opt/Log.hpp"

#include <unistd.h>
#include <string.h>

namespace zyb
{
    IMPLEMENT_SINGLETON(CThreadPool)
    {
        m_pArrLock          = 0;
        m_pArrThreadCond    = 0;
        m_pArrThread        = 0;
        m_nThreadNum        = 0;
        m_bInit             = false;
        m_bRun              = false;
        m_pArrTaskQueue     = 0;
        m_pArrThreadParamer = 0;
        m_nInputIndex       = 0;
    }
    
    CThreadPool::~CThreadPool() 
    {
        ClearThreadPool();
    }
    
    bool CThreadPool::InitThreadPool()
    {
        bool bRet = false;
        for (;;)
        {
            if (m_bInit)
            {
                LOG_ERROR("第二次初始化 CThreadPool，请检查");
                bRet = true;
                break;
            }
            m_nInputIndex       = 0;
            
            m_nThreadNum = sysconf(_SC_NPROCESSORS_CONF) * 2;
            if (0 >= m_nThreadNum) 
            {
                LOG_ERROR("获取处理器的数量失败，数量为%d", m_nThreadNum);
                break;
            }
            m_pArrThreadParamer = new _tagThreadParamer[m_nThreadNum];
            if (!m_pArrThreadParamer) 
            {
                LOG_ERROR("申请内存失败");
                break;
            }
            m_pArrLock = new CLock[m_nThreadNum];
            if (!m_pArrLock) 
            {
                LOG_ERROR("申请内存失败");
                break;
            }
            m_pArrTaskQueue = new CTaskQueue[m_nThreadNum];
            if (!m_pArrTaskQueue)
            {
                LOG_ERROR("申请内存失败");
                break;
            }
            
            m_pArrThreadCond = new pthread_cond_t[m_nThreadNum];
            if (!m_pArrThreadCond)
            {
                LOG_ERROR("申请内存失败");
                break;
            }
            for (int i = 0; i < m_nThreadNum; i++) 
            {
                m_pArrThreadCond[i] = PTHREAD_COND_INITIALIZER;
            }
            
            m_pArrThread = new pthread_t[m_nThreadNum];
            if (!m_pArrThread) 
            {
                LOG_ERROR("申请内存失败");
                break;
            }
            memset(m_pArrThread, 0, m_nThreadNum * sizeof (pthread_t));
            
            m_bRun = true;
            bool bCreateAllThread = true;
            for (int i = 0; i < m_nThreadNum; i++) 
            {
                m_pArrThreadParamer[i].nThreadIndex = i;
                m_pArrThreadParamer[i].pThis = this;

                if (pthread_create(&m_pArrThread[i], 0, &CThreadPool::WorkThread, &m_pArrThreadParamer[i])) 
                {
                    LOG_ERROR("开启第%d个线程失败， 总共需要开启%d个线程", i, m_nThreadNum);
                    bCreateAllThread = false;
                    break;
                }
            }
            if (!bCreateAllThread) 
            {
                break;
            }
            
            m_bInit = true;
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CThreadPool::PushTaskToBack(CTask *pTask)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit || !pTask) 
            {
                LOG_ERROR("没有初始化或参数错误");
                break;
            }
            
            int nIndex = GetCurrentTaskQueueIndex();
            int nTaskNums = m_pArrTaskQueue[nIndex].PushTaskToBack(pTask);
            if (-1 == nTaskNums)
            {
                pTask ->FreeTask();
                break;
            }
            if (1 == nTaskNums)
            {
                CAutoLock _autoLock(&m_pArrLock[nIndex]);
                pthread_cond_signal(&m_pArrThreadCond[nIndex]);
            }

            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CThreadPool::PushTaskToFront(CTask *pTask)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit || !pTask) 
            {
                LOG_ERROR("没有初始化或参数错误");
                break;
            }
            
            int nIndex = GetCurrentTaskQueueIndex();
            int nTaskNums = m_pArrTaskQueue[nIndex].PushTaskToFront(pTask);
            if (-1 == nTaskNums)
            {
                pTask ->FreeTask();
                break;
            }
            if (1 == nTaskNums)
            {
                CAutoLock _autoLock(&m_pArrLock[nIndex]);
                pthread_cond_signal(&m_pArrThreadCond[nIndex]);
            }

            bRet = true;
            break;
        }
        return bRet;
    }
    
    void *CThreadPool::WorkThread(void *pParam)
    {
        _tagThreadParamer *pParamer = (_tagThreadParamer *) pParam;
        CThreadPool *pThis = pParamer ->pThis;
        int nThreadIndex = pParamer ->nThreadIndex;
        
        while (pThis ->m_bRun)
        {
            CTask *pTask = 0;
            if (1)
            {
                CAutoLock _autoLock(&pThis ->m_pArrLock[nThreadIndex]);
                pTask = pThis ->m_pArrTaskQueue[nThreadIndex].PopTask();
                if (!pTask)
                {
                    pthread_cond_wait(&pThis ->m_pArrThreadCond[nThreadIndex], pThis ->m_pArrLock[nThreadIndex].GetMutex());
                    continue;
                }
            }
            
            pTask ->Run();
            pTask ->FreeTask();
        }
        return 0;
    }
    
    void CThreadPool::ClearThreadPool()
    {
        m_bInit = false;
        m_bRun = false;
        void *pExit = 0;
        for (int i = 0; i < m_nThreadNum; i++) 
        {
            pthread_cond_signal(&m_pArrThreadCond[i]);
            if (m_pArrThread[i]) 
            {
                pthread_join(m_pArrThread[i], &pExit);
            }
            pthread_cond_destroy(&m_pArrThreadCond[i]);
            m_pArrThreadCond[i] = PTHREAD_COND_INITIALIZER;
        }
        delete []m_pArrThread;
        m_pArrThread = 0;
        m_nThreadNum = 0;

        if (m_pArrTaskQueue) 
        {
            delete []m_pArrTaskQueue;
            m_pArrTaskQueue = 0;
        }

        if (m_pArrThreadParamer) 
        {
            delete []m_pArrThreadParamer;
            m_pArrThreadParamer = 0;
        }
        if (m_pArrLock) 
        {
            delete []m_pArrLock;
            m_pArrLock = 0;
        }
        if (m_pArrThreadCond) 
        {
            delete []m_pArrThreadCond;
            m_pArrThreadCond = 0;
        }
    }
    
    int CThreadPool::GetCurrentTaskQueueIndex()
    {
        int nRet = 0;
        for (;;)
        {
            CAutoLock _lock(&m_InputIndexLock);
            nRet = m_nInputIndex ++;
            if (m_nInputIndex >= m_nThreadNum)
            {
                m_nInputIndex = 0;
            }
            break;
        }
        return nRet;
    }
}