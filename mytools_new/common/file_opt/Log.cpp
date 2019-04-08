#include "Log.hpp"
#include <stdarg.h>
#include <string.h>

namespace zyb 
{
    IMPLEMENT_SINGLETON(CLog) 
    {
        m_pLogFile = 0;
        m_bInit = false;
        m_lv = emLog_UNKNOW;
    }

    CLog::~CLog() 
    {
        m_bInit = false;
        if (!m_pLogFile) 
        {
            fclose(m_pLogFile);
            m_pLogFile = 0;
        }
    }

    bool CLog::InitLog(MENU_LOG_LEVEL lv, const char *pLogPath) 
    {
        bool bRet = false;
        for (;;) 
        {
            if (m_bInit) 
            {
                bRet = true;
                break;
            }

            if (
                 emLog_ERROR == lv ||
                 emLog_WARNING == lv ||
                    emLog_DEBUG == lv ||
                    emLog_INFO == lv
               ) 
            {
                m_lv = lv;
            } 
            else 
            {
                break;
            }

            if (!pLogPath || 0 == pLogPath) 
            {
                break;
            }

            m_pLogFile = fopen(pLogPath, "a+");
            if (!m_pLogFile) 
            {
                break;
            }

            m_bInit = true;
            bRet = true;
            break;
        }
        return bRet;
    }
    
    MENU_LOG_LEVEL CLog::GetLogLevel() const
    {
        return m_lv;
    }

    bool CLog::WritLog(MENU_LOG_LEVEL lv, const char *pInfo, ...) 
    {
        bool bRet = false;
        for (;;) 
        {
            if (!m_bInit) 
            {
                break;
            }

            if (lv > m_lv || lv < emLog_ERROR) 
            {
                break;
            }

            char szLog[MAX_LOG_LEN];
            va_list args;
            va_start(args, pInfo);
            vsnprintf(szLog, MAX_LOG_LEN, pInfo, args);
            va_end(args);

            const char *pLevel = 0;
            switch (lv) 
            {
                case emLog_ERROR:
                {
                    pLevel = "[ERROR]:   %s \r\n";
                    break;
                }
                case emLog_WARNING:
                {
                    pLevel = "[WARNING]: %s \r\n";
                    break;
                }
                case emLog_DEBUG:
                {
                    pLevel = "[DEBUG]:   %s \r\n";
                    break;
                }
                case emLog_INFO:
                {
                    pLevel = "[INFO]:    %s \r\n";
                    break;
                }
                default:
                {
                    break;
                }
            }

            if (pLevel) 
            {
                CAutoLock _autoLock(&m_lock);
                fprintf(m_pLogFile, pLevel, szLog);
                fflush(m_pLogFile);
            }

            bRet = true;
            break;
        }
        return bRet;
    }

}