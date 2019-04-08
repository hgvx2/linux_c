#ifndef TASK_QUEUE_HPP
#define TASK_QUEUE_HPP

#include <list>
#include "../lock/Lock.hpp"
#include "Task.hpp"


namespace zyb
{
    class CTaskManager;
    class CTaskQueue
    {
    public:
        CTaskQueue();
        ~CTaskQueue();
        
        // 添加任务到队列， 如果失败返回-1， 如果成功返回当前队列中任务的数量
        int PushTaskToBack(CTask *pTask);
        int PushTaskToFront(CTask *pTask);
        
        CTask *PopTask(); 
        
        // 如果失败返回-1， 如果成功返回当前队列中任务的数量
        int GetTaskNumbers();
    private:
        CLock m_lock;
        std::list<CTask *> m_listTaskQueue;
    };
}

#endif // TASK_QUEUE_HPP