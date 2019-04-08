#include "TcpSrvTask.hpp"
#include "../../../file_opt/Log.hpp"

namespace zyb
{
    
    CTcpSrvTask::CTcpSrvTask(CTaskManager *pTaskManager, int nTaskType):
    CTask(pTaskManager, nTaskType)
    {
        
    }
    CTcpSrvTask::~CTcpSrvTask()
    {
        
    }
    
    bool CTcpSrvTask::Run()
    {
        bool bRet = false;
        for (;;)
        {
            switch (GetTaskType())
            {
                case TASK_TYPE_NEED_CUT:
                case TASK_TYPE_PROC:
                {
                    bRet = m_fun(this);
                    break;
                }
                default:
                {
                    LOG_ERROR("出现错误， 没有找到指定的类型 %d", GetTaskType());
                    break;
                }
            }
            break;
        }
        return bRet;
    }
    
    void CTcpSrvTask::InitSrvTask(SrvTaskCallback fun)
    {
        m_fun = fun;
    }
    
    CTcpSrvNeedCutTask::CTcpSrvNeedCutTask(CTaskManager* pTaskManager):
    CTcpSrvTask(pTaskManager, TASK_TYPE_NEED_CUT)
    {
        
    }
    
    CTcpSrvNeedCutTask::~CTcpSrvNeedCutTask()
    {
        
    }
    
    void CTcpSrvNeedCutTask::InitSrvNeedCutTask(SrvTaskCallback fun, SOCKET_FD fd)
    {
        CTcpSrvTask::InitSrvTask(fun);
        m_fd = fd;
    }
    
    SOCKET_FD CTcpSrvNeedCutTask::GetSocketFD() const
    {
        return m_fd;
    }
    

    CTcpSrvProcTask::CTcpSrvProcTask(CTaskManager* pTaskManager):
                    CTcpSrvTask(pTaskManager, TASK_TYPE_PROC)
    {
        
    }
    
    CTcpSrvProcTask::~CTcpSrvProcTask()
    {
        
    }
    
    void CTcpSrvProcTask::InitSrvProcTask(SrvTaskCallback fun, CBinaryBuf* pBuf, SOCKET_FD fd)
    {
        CTcpSrvTask::InitSrvTask(fun);
        m_pBuf = pBuf;
        m_fd   = fd;
    }
    
    SOCKET_FD CTcpSrvProcTask::GetSocketFD() const
    {
        return m_fd;
    }
    
    CBinaryBuf  *CTcpSrvProcTask::GetBuf() const
    {
        return m_pBuf;
    }
}