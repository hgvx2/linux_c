#include "UdpSrvTaskManager.hpp"
#include "UdpSrvTask.hpp"
#include "../../../file_opt/Log.hpp"

namespace zyb
{
    CUdpSrvTaskManager::CUdpSrvTaskManager():
    m_poolProcTask(10, this),
    m_poolFDProcTask(10, this)
    {
        
    }
    
    CUdpSrvTaskManager::~CUdpSrvTaskManager()
    {
        
    }
    
    CTask *CUdpSrvTaskManager::ProvideTask(int nTaskTpye)
    {
        CTask *pRet = 0;
        for (;;)
        {
            switch (nTaskTpye)
            {
                case TASK_TYPE_PROC:
                {
                    pRet = m_poolProcTask.ProvideData();
                    break;
                }
                case TASK_TYPE_FD_PROC:
                {
                    pRet = m_poolFDProcTask.ProvideData();
                    break;
                }
                default:
                {
                    LOG_ERROR("出现错误， 没有找到指定的类型 %d", nTaskTpye);
                    break;
                }
            }
            break;
        }
        return pRet;
    }
    
    bool CUdpSrvTaskManager::RecoveryTask(CTask *pTask)
    {
        bool bRet = false;
        for (;;)
        {
            switch (pTask ->GetTaskType())
            {
                case TASK_TYPE_PROC:
                {
                    CUdpSrvProcTask *pTemp = dynamic_cast<CUdpSrvProcTask *>(pTask);
                    if (!pTemp)
                    {
                        LOG_ERROR("出现错误， pTask to CSrvProcTask failed");
                        break;
                    }
                    bRet = m_poolProcTask.DestroyData(pTemp);
                    break;
                }
                case TASK_TYPE_FD_PROC:
                {
                    CUdpSrvFdProcTask *pTemp = dynamic_cast<CUdpSrvFdProcTask *>(pTask);
                    if (!pTemp)
                    {
                        LOG_ERROR("出现错误， pTask to CUdpSrvFdProcTask failed");
                        break;
                    }
                    bRet = m_poolFDProcTask.DestroyData(pTemp);
                    break;
                }
                default:
                {
                    LOG_ERROR("出现错误， 没有找到指定的类型 %d", pTask ->GetTaskType());
                    break;
                }
            }
            break;
        }
        return bRet;
    }
}