#ifndef TASK_HPP
#define TASK_HPP

namespace zyb
{
    class CTaskManager;
    class CTask
    {
    public:
        CTask(CTaskManager *pTaskManager, int nTaskType);
        virtual ~CTask();
        
        virtual bool Run() = 0;
        
        void FreeTask();
        int GetTaskType() const;
    private:
        CTaskManager *m_pTaskManager;
        int           m_nTaskType;
    };
}

#endif // TASK_HPP