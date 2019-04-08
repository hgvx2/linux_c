/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TestSrv.hpp"
#include <stdio.h>

namespace zyb
{
    CTestSrv::CTestSrv()
    {
        
    }
    CTestSrv::~CTestSrv()
    {
        
    }
        
    bool CTestSrv::ProcAcceptFD(SOCKET_FD fd)
    {
        return SendData(fd, "0123456789", 11);
    }
    bool CTestSrv::ProcConnectFD(SOCKET_FD fd)
    {
        return true;
    }
    bool CTestSrv::ProcDisableFD(SOCKET_FD fd)
    {
        return true;
    }
    bool CTestSrv::ProcPack(SOCKET_FD fd, const char * pData, int nSize)
    {
        //printf("ProcPack fd=%d \n", fd);
        bool bRet = false;
        for (;;)
        {
            bRet = this ->SendData(fd, pData, nSize);
            break;
        }
        //this ->CloseSocketFD(fd);
        return bRet;
    }
}
