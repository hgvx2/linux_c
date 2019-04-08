/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TestSrv.hpp
 * Author: zuo
 *
 * Created on August 22, 2018, 8:49 PM
 */

#ifndef TESTSRV_HPP
#define TESTSRV_HPP

#include "../common/srv/udp/UdpSrv.hpp"

namespace zyb
{
    class CTestSrv : public CUdpSrv
    {
    public:
        CTestSrv();
        ~CTestSrv();
        
    private:
        virtual bool ProcAcceptFD(SOCKET_FD fd);
        virtual bool ProcConnectFD(SOCKET_FD fd);
        virtual bool ProcDisableFD(SOCKET_FD fd);
        virtual bool ProcPack(SOCKET_FD fd, const char * pData, int nSize);
    };
}


#endif /* TESTSRV_HPP */

