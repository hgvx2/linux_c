/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "TestSrv.hpp"
#include "../common/file_opt/Log.hpp"
#include <stdio.h>
namespace zyb
{
    CTestSrv::CTestSrv()
    {
        
    }
    CTestSrv::~CTestSrv()
    {
        
    }
        
    bool CTestSrv::ProcAcceptFD(SOCKET_FD fd, const char * pData, int nSize)
    {
        LOG_ERROR("fd=%d  %s", fd, pData ? pData : "");
        m_onlineSocket.insert(std::pair<SOCKET_FD,int>(fd, 1));
        return true;
    }
    bool CTestSrv::ProcConnectFD(SOCKET_FD fd)
    {
        
        return true;
    }
    bool CTestSrv::ProcDisableFD(SOCKET_FD fd)
    {
        m_onlineSocket.erase(fd);
        LOG_ERROR("close socket fd=%d", fd);
        return true;
    }
    bool CTestSrv::ProcPack(SOCKET_FD fd, const char * pData, int nSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (m_onlineSocket.find(fd) == m_onlineSocket.end())
            {
                printf("socket close fd=%d unproc data\n", fd);
                break;
            }
            bRet = this ->SendData(fd, pData, nSize);
            break;
        }
        return bRet;
    }
}
