/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: zuo
 *
 * Created on August 20, 2018, 2:16 PM
 */

#include "../common/file_opt/Log.hpp"
#include "TestSrv.hpp"

#include <cstdlib>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv)
{

    zyb::CLog *pLog = zyb::CLog::GetInstance();
    if (!pLog)
    {
        printf("----log err \n");
        return 0;
    }
    
    if (!pLog ->InitLog(zyb::emLog_INFO, "/home/zuo/log.txt"))
    {
        printf("----InitLog err \n");
        return 0;
    }
    
    zyb::CTestSrv srv;
    if (!srv.InitUdpSrv(0, 8000))
    {
        printf("InitSrvBase error!\n");
        return 0;
    }
    
    srv.RunTcpSrv();
    return 0;
}

