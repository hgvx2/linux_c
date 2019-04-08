#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <pthread.h>
#include "../design_patterns/Singleton.hpp"
#include "../lock/Lock.hpp"
#include "Task.hpp"

namespace zyb
{
    class CTaskQueue;
    class CThreadPool;
    struct _tagThreadParamer 
    {
        CThreadPool *pThis;
        int nThreadIndex;
    };
    
    class CThreadPool
    {
        DECLARE_SINGLETON(CThreadPool)
                
    public:
        ~CThreadPool();
        
        bool InitThreadPool();
        
        bool PushTaskToBack(CTask *pTask);
        bool PushTaskToFront(CTask *pTask);
        
    private:
        static void *WorkThread(void *pParam);
        void ClearThreadPool();
        
        int  GetCurrentTaskQueueIndex();
        
        
        CLock *m_pArrLock;
        pthread_cond_t *m_pArrThreadCond;
        pthread_t *m_pArrThread;
        int m_nThreadNum;
        bool m_bInit;
        bool m_bRun;
        CTaskQueue *m_pArrTaskQueue;
        _tagThreadParamer *m_pArrThreadParamer;
        
        int m_nInputIndex;
        CLock m_InputIndexLock;
    };
}

#endif // THREAD_POOL_HPP