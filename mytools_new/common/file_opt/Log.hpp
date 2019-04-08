#ifndef __LOG_HPP__
#define __LOG_HPP__

#include <stdio.h>
#include "../design_patterns/Singleton.hpp"
#include "../lock/Lock.hpp"

namespace zyb 
{
    #define MAX_LOG_LEN  (2048)

    enum MENU_LOG_LEVEL 
    {
        emLog_UNKNOW = -1,
        emLog_ERROR = 0,
        emLog_WARNING,
        emLog_DEBUG,
        emLog_INFO
    };

    class CLog 
    {
        DECLARE_SINGLETON(CLog)

    public:
        ~CLog();

        bool InitLog(MENU_LOG_LEVEL lv, const char *pLogPath);
        MENU_LOG_LEVEL GetLogLevel()const;

        bool WritLog(MENU_LOG_LEVEL lv, const char *pInfo, ...);

    private:
        CLock m_lock;
        FILE *m_pLogFile;
        bool m_bInit;

        MENU_LOG_LEVEL m_lv;
    };

}

#define USE_LOG 1

#ifdef USE_LOG

#define LOG_WRITE(t, x, ...) do \
{ \
   if (zyb::CLog::GetInstance() ->GetLogLevel() >= t) \
   { \
       char szTemp[MAX_LOG_LEN] = {0}; \
       snprintf(szTemp, MAX_LOG_LEN, "[%s %s][file:%s][line:%d] %s", __DATE__, __TIME__, __FILE__, __LINE__, x); \
       zyb::CLog::GetInstance() ->WritLog(t, szTemp, ## __VA_ARGS__); \
   } \
}while(0);
 
 
#define LOG_ERROR(x, ...)   LOG_WRITE(emLog_ERROR, x, ## __VA_ARGS__)
#define LOG_WARNING(x, ...) LOG_WRITE(emLog_WARNING, x, ## __VA_ARGS__)
#define LOG_DEBUG(x, ...)   LOG_WRITE(emLog_DEBUG, x, ## __VA_ARGS__)
#define LOG_INFO(x, ...)    LOG_WRITE(emLog_INFO, x, ## __VA_ARGS__)

#else

#define LOG_ERROR(x, ...)   printf(x, ## __VA_ARGS__)
#define LOG_WARNING(x, ...) printf(x, ## __VA_ARGS__)
#define LOG_DEBUG(x, ...)   printf(x, ## __VA_ARGS__)
#define LOG_INFO(x, ...)    printf(x, ## __VA_ARGS__)
 
#endif // USE_LOG


#endif // __LOG_HPP__