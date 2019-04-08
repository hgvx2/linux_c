#include "Task.hpp"
#include "TaskManager.hpp"

namespace zyb
{
    CTask::CTask(CTaskManager* pTaskManager, int nTaskType)
    {
        m_pTaskManager = pTaskManager;
        m_nTaskType    = nTaskType;
    }
    
    CTask::~CTask()
    {
        
    }
    
    void CTask::FreeTask()
    {
        m_pTaskManager ->RecoveryTask(this);
    }
    
    int CTask::GetTaskType() const
    {
        return m_nTaskType;
    }
}