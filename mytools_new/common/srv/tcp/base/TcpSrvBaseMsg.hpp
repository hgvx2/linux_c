#ifndef TCP_SRV_BASE_MSG_HPP
#define TCP_SRV_BASE_MSG_HPP

namespace zyb
{
    typedef enum _TCP_SRV_BASE_MSG
    {
        enumTSBM_ACCEPT     = 1,
        enumTSBM_CONNECT    = 2,  
        enumTSBM_DISCONNECT = 3,
        enumTSBM_NEED_CUT   = 4,
        enumTSBM_CUT        = 5
    }TCP_SRV_BASE_MSG;
}

#endif //TCP_SRV_BASE_MSG_HPP