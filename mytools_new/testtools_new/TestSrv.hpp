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

#include "../common/srv/tcp/TcpSrv.hpp"
#include <map>

namespace zyb
{
    class CTestSrv : public CTcpSrv
    {
    public:
        CTestSrv();
        ~CTestSrv();
        
    private:
        virtual bool ProcAcceptFD(SOCKET_FD fd, const char * pData, int nSize);
        virtual bool ProcConnectFD(SOCKET_FD fd);
        virtual bool ProcDisableFD(SOCKET_FD fd);
        virtual bool ProcPack(SOCKET_FD fd, const char * pData, int nSize);
        
        std::map<SOCKET_FD, int> m_onlineSocket;
    };
}


#endif /* TESTSRV_HPP */

