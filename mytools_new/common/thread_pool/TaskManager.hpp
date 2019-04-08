#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

namespace zyb
{
    class CTask;
    class CTaskManager
    {
    public:
        CTaskManager();
        virtual ~CTaskManager();
        
        virtual CTask *ProvideTask(int nTaskTpye) = 0;
        virtual bool RecoveryTask(CTask *pTask) = 0;
    };
}

#endif // TASK_MANAGER_HPP