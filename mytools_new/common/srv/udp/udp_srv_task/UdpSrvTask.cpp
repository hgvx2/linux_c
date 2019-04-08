#include "UdpSrvTask.hpp"
#include "../../../file_opt/Log.hpp"

namespace zyb
{
    
    CUdpSrvTask::CUdpSrvTask(CTaskManager *pTaskManager, int nTaskType):
    CTask(pTaskManager, nTaskType)
    {
        
    }
    CUdpSrvTask::~CUdpSrvTask()
    {
        
    }
    
    bool CUdpSrvTask::Run()
    {
        bool bRet = false;
        for (;;)
        {
            switch (GetTaskType())
            {
                case TASK_TYPE_FD_PROC:
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
    
    void CUdpSrvTask::InitSrvTask(UdpSrvTaskCallback fun)
    {
        m_fun = fun;
    }
    
    
    CUdpSrvFdProcTask::CUdpSrvFdProcTask(CTaskManager* pTaskManager):
                    CUdpSrvTask(pTaskManager, TASK_TYPE_FD_PROC)
    {
        
    }
    
    CUdpSrvFdProcTask::~CUdpSrvFdProcTask()
    {
        
    }
    
    void CUdpSrvFdProcTask::InitSrvFDProcTask(UdpSrvTaskCallback fun, SOCKET_FD fd)
    {
        CUdpSrvTask::InitSrvTask(fun);
        m_fd   = fd;
    }
    
    SOCKET_FD CUdpSrvFdProcTask::GetSocketFD() const
    {
        return m_fd;
    }

    CUdpSrvProcTask::CUdpSrvProcTask(CTaskManager* pTaskManager):
                    CUdpSrvTask(pTaskManager, TASK_TYPE_PROC)
    {
        
    }
    
    CUdpSrvProcTask::~CUdpSrvProcTask()
    {
        
    }
    
    void CUdpSrvProcTask::InitSrvProcTask(UdpSrvTaskCallback fun, CBinaryBuf* pBuf, SOCKET_FD fd)
    {
        CUdpSrvTask::InitSrvTask(fun);
        m_pBuf = pBuf;
        m_fd   = fd;
    }
    
    SOCKET_FD CUdpSrvProcTask::GetSocketFD() const
    {
        return m_fd;
    }
    
    CBinaryBuf  *CUdpSrvProcTask::GetBuf() const
    {
        return m_pBuf;
    }
}