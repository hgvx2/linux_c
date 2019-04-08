#ifndef UDP_SRV_TASK_MANAGER_HPP
#define UDP_SRV_TASK_MANAGER_HPP

#include "../../../thread_pool/TaskManager.hpp"
#include "../../../memory_pool/MemoryPool.hpp"
#include "UdpSrvTask.hpp"

namespace zyb
{
    class CUdpSrvTaskManager : public CTaskManager
    {
    public:
        CUdpSrvTaskManager();
        virtual ~CUdpSrvTaskManager();
        
        virtual CTask *ProvideTask(int nTaskTpye);
        virtual bool RecoveryTask(CTask *pTask);
        
    private:
        CMemoryPool1<CUdpSrvProcTask, CTaskManager*> m_poolProcTask;
        CMemoryPool1<CUdpSrvFdProcTask, CTaskManager*> m_poolFDProcTask;
    };
}


#endif // UDP_SRV_TASK_MANAGER_HPP