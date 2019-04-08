#ifndef TCP_SRV_TASK_MANAGER_HPP
#define TCP_SRV_TASK_MANAGER_HPP

#include "../../../thread_pool/TaskManager.hpp"
#include "../../../memory_pool/MemoryPool.hpp"
#include "TcpSrvTask.hpp"

namespace zyb
{
    class CTcpSrvTaskManager : public CTaskManager
    {
    public:
        CTcpSrvTaskManager();
        virtual ~CTcpSrvTaskManager();
        
        virtual CTask *ProvideTask(int nTaskTpye);
        virtual bool RecoveryTask(CTask *pTask);
        
    private:
        CMemoryPool1<CTcpSrvNeedCutTask, CTaskManager*> m_poolNeedCutTask;
        CMemoryPool1<CTcpSrvProcTask, CTaskManager*> m_poolProcTask;
    };
}


#endif //TCP_SRV_TASK_MANAGER_HPP