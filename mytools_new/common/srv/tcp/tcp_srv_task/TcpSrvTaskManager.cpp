#include "TcpSrvTaskManager.hpp"
#include "TcpSrvTask.hpp"
#include "../../../file_opt/Log.hpp"

namespace zyb
{
    CTcpSrvTaskManager::CTcpSrvTaskManager():
    m_poolNeedCutTask(10, this),
    m_poolProcTask(10, this)
    {
        
    }
    
    CTcpSrvTaskManager::~CTcpSrvTaskManager()
    {
        
    }
    
    CTask *CTcpSrvTaskManager::ProvideTask(int nTaskTpye)
    {
        CTask *pRet = 0;
        for (;;)
        {
            switch (nTaskTpye)
            {
                case TASK_TYPE_NEED_CUT:
                {
                    pRet = m_poolNeedCutTask.ProvideData();
                    break;
                }
                case TASK_TYPE_PROC:
                {
                    pRet = m_poolProcTask.ProvideData();
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
    
    bool CTcpSrvTaskManager::RecoveryTask(CTask *pTask)
    {
        bool bRet = false;
        for (;;)
        {
            switch (pTask ->GetTaskType())
            {
                case TASK_TYPE_NEED_CUT:
                {
                    CTcpSrvNeedCutTask *pTemp = dynamic_cast<CTcpSrvNeedCutTask *>(pTask);
                    if (!pTemp)
                    {
                        LOG_ERROR("出现错误， pTask to CSrvNeedCutTask failed");
                        break;
                    }
                    bRet = m_poolNeedCutTask.DestroyData(pTemp);
                    break;
                }
                case TASK_TYPE_PROC:
                {
                    CTcpSrvProcTask *pTemp = dynamic_cast<CTcpSrvProcTask *>(pTask);
                    if (!pTemp)
                    {
                        LOG_ERROR("出现错误， pTask to CSrvProcTask failed");
                        break;
                    }
                    bRet = m_poolProcTask.DestroyData(pTemp);
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