#include "TaskQueue.hpp"

namespace zyb
{
    CTaskQueue::CTaskQueue()
    {
        m_listTaskQueue.clear();
    }
    
    CTaskQueue::~CTaskQueue()
    {
        m_listTaskQueue.clear();
    }
    
    int CTaskQueue::PushTaskToBack(CTask* pTask)
    {
        int nRet = -1;
        for (;;)
        {
            CAutoLock _lock(&m_lock);
            m_listTaskQueue.push_back(pTask);
            nRet = (int)m_listTaskQueue.size();
            break;
        }
        return nRet;
    }
    
    int CTaskQueue::PushTaskToFront(CTask* pTask)
    {
        bool nRet = -1;
        for (;;)
        {
            CAutoLock _lock(&m_lock);
            m_listTaskQueue.push_front(pTask);
            nRet = (int)m_listTaskQueue.size();
            break;
        }
        return nRet;
    }
    
    CTask *CTaskQueue::PopTask()
    {
        CTask *pRet = 0;
        for (;;)
        {
            CAutoLock _lock(&m_lock);
            if (m_listTaskQueue.empty())
            {
                break;
            }
            pRet = *m_listTaskQueue.begin();
            m_listTaskQueue.pop_front();
            
            break;
        }
        return pRet;
    }
    
    int CTaskQueue::GetTaskNumbers()
    {
        int nRet = -1;
        for (;;)
        {
            CAutoLock _lock(&m_lock);
            nRet = (int)m_listTaskQueue.size();
            break;
        }
        return nRet;
    }
}