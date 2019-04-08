#ifndef TCP_SRV_TASK_HPP
#define TCP_SRV_TASK_HPP

#include "../../../thread_pool/Task.hpp"
#include "../../../epoll/SocketDefault.hpp"
#include "../../../buffer/BinaryBuf.hpp"

namespace zyb
{
    
#define TASK_TYPE_NEED_CUT  (0)
#define TASK_TYPE_PROC      (1)
    
    typedef bool (*SrvTaskCallback)(CTask *);
    
    class CTcpSrvTask : public CTask
    {
    public:
        CTcpSrvTask(CTaskManager *pTaskManager, int nTaskType);
        virtual ~CTcpSrvTask();
        
        virtual bool Run();
        
        void InitSrvTask(SrvTaskCallback fun);
    private:
        SrvTaskCallback m_fun;
    };
    
    class CTcpSrvNeedCutTask : public CTcpSrvTask
    {
    public:
        CTcpSrvNeedCutTask(CTaskManager *pTaskManager);
        virtual ~CTcpSrvNeedCutTask();
        
        void InitSrvNeedCutTask(SrvTaskCallback fun, SOCKET_FD fd);
        
        SOCKET_FD GetSocketFD() const;
    private:
        SOCKET_FD  m_fd;
    };
    
    class CTcpSrvProcTask : public CTcpSrvTask
    {
    public:
        CTcpSrvProcTask(CTaskManager *pTaskManager);
        virtual ~CTcpSrvProcTask();
        
        void InitSrvProcTask(SrvTaskCallback fun, CBinaryBuf *pBuf, SOCKET_FD fd);
        SOCKET_FD GetSocketFD() const;
        
        CBinaryBuf  *GetBuf() const;
    private:
        CBinaryBuf  *m_pBuf;
        SOCKET_FD  m_fd;
    };
    
}


#endif // TCP_SRV_TASK_HPP