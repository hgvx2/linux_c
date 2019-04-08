#ifndef UDP_SRV_TASK_HPP
#define UDP_SRV_TASK_HPP

#include "../../../thread_pool/Task.hpp"
#include "../../../epoll/SocketDefault.hpp"
#include "../../../buffer/BinaryBuf.hpp"

namespace zyb
{
#define TASK_TYPE_PROC         (1)
#define TASK_TYPE_FD_PROC      (2)
    
    typedef bool (*UdpSrvTaskCallback)(CTask *);
    
    class CUdpSrvTask : public CTask
    {
    public:
        CUdpSrvTask(CTaskManager *pTaskManager, int nTaskType);
        virtual ~CUdpSrvTask();
        
        virtual bool Run();
        
        void InitSrvTask(UdpSrvTaskCallback fun);
    private:
        UdpSrvTaskCallback m_fun;
    };
    
    class CUdpSrvFdProcTask : public CUdpSrvTask
    {
    public:
        CUdpSrvFdProcTask(CTaskManager *pTaskManager);
        virtual ~CUdpSrvFdProcTask();
        
        void InitSrvFDProcTask(UdpSrvTaskCallback fun, SOCKET_FD fd);
        SOCKET_FD GetSocketFD() const;
    private:
        SOCKET_FD  m_fd;
    };
    
    class CUdpSrvProcTask : public CUdpSrvTask
    {
    public:
        CUdpSrvProcTask(CTaskManager *pTaskManager);
        virtual ~CUdpSrvProcTask();
        
        void InitSrvProcTask(UdpSrvTaskCallback fun, CBinaryBuf *pBuf, SOCKET_FD fd);
        SOCKET_FD GetSocketFD() const;
        
        CBinaryBuf  *GetBuf() const;
    private:
        CBinaryBuf  *m_pBuf;
        SOCKET_FD  m_fd;
    };
    
}


#endif // UDP_SRV_TASK_HPP