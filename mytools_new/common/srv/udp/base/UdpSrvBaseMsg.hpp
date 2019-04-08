#ifndef UDP_SRV_BASE_MSG_HPP
#define UDP_SRV_BASE_MSG_HPP

namespace zyb
{
    typedef enum _DUP_SRV_BASE_MSG
    {
        enumUSBM_ACCEPT     = 1,
        enumUSBM_CONNECT    = 2,  
        enumUSBM_DISCONNECT = 3,
        enumUSBM_PROC_PACK  = 4,
        enumUSBM_VALIDATE   = 5   
    }UDP_SRV_BASE_MSG;
}

#endif //UDP_SRV_BASE_MSG_HPP