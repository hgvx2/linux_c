#ifndef PING_PONG_CACHE_HPP
#define PING_PONG_CACHE_HPP

#include "BinaryBuf.hpp"
#include "../lock/Lock.hpp"
#include <queue>

namespace zyb
{
    typedef enum _PING_PONG_CODE
    {
        enumPPC_ERROR = 0,
        enumPPC_OK    = 1,    // 正常
        enumPPC_ET    = 2     // 触发 
    }PING_PONG_CODE;
        
    template<typename T, typename P>
    class CPingPongCache
    {
        typedef void (T::*DestoryFun)(P);
    public:
        CPingPongCache(DestoryFun fun, T* pt);
        ~CPingPongCache();
        
        PING_PONG_CODE Push(P p1);
        P  Pop(P defaultVal);
        void ClearCache();
    private:
        std::queue<P> m_inputCache;
        std::queue<P> m_outputCache;
        
        std::queue<P> *m_pInputCache;
        std::queue<P> *m_pOutputCache;
        
        DestoryFun m_pFun;
        T* m_pT;
        CLock m_inputLock;
        CLock m_outputLock;
        bool m_bIsPopRunning;  // 用于边缘触发
    };
    
    template<typename T, typename P>
    CPingPongCache<T, P>::CPingPongCache(DestoryFun fun, T* pt) 
    {
        m_pFun = fun;
        m_pT = pt;
        if (1)
        {
            CAutoLock _lock(&m_inputLock);
            m_pInputCache  = &m_inputCache;
        }
        if (1)
        {
            CAutoLock _lock(&m_outputLock);
            m_pOutputCache = &m_outputCache;
        }
        
        m_bIsPopRunning = false;
    }
    template<typename T, typename P>
    CPingPongCache<T, P>::~CPingPongCache()
    {
        ClearCache();
        
        m_pInputCache    = 0;
        m_pOutputCache   = 0;
        m_pFun = 0;
    }
    
    template<typename T, typename P>
    PING_PONG_CODE CPingPongCache<T, P>::Push(P p1)
    {
        PING_PONG_CODE code = enumPPC_ERROR;
        for (;;)
        {
            CAutoLock _lock(&m_inputLock);
            m_pInputCache->push(p1);
            if (1 == m_pInputCache ->size() && !m_bIsPopRunning)
            {
                CAutoLock _lock(&m_outputLock);
                if (0 <= m_pOutputCache ->size())
                {
                    // 触发
                    code = enumPPC_ET;
                    break;
                }
            }
            code = enumPPC_OK;
            break;
        }
        return code;
    }
    
    template<typename T, typename P>
    P CPingPongCache<T, P>::Pop(P defaultVal)
    {
        P ret = defaultVal;
        for (;;)
        {
            if (!m_bIsPopRunning)
            {
                CAutoLock _lock(&m_inputLock);
                m_bIsPopRunning = true;
            }
            CAutoLock _lock(&m_outputLock);
            if (m_pOutputCache ->empty())
            {
                CAutoLock _lock(&m_inputLock);
                if (!m_pInputCache ->empty())
                {
                    std::queue<P> *pTemp = m_pOutputCache;
                    m_pOutputCache = m_pInputCache;
                    m_pInputCache = pTemp;
                }
                else
                {
                    m_bIsPopRunning = false;
                    break;
                }
            }
            ret = m_pOutputCache ->front();
            m_pOutputCache ->pop();
            break;
        }
        return ret;
    }
    
    template<typename T, typename P>
    void CPingPongCache<T, P>::ClearCache()
    {
        if (1)
        {
            CAutoLock _lock(&m_inputLock);
            while (!m_inputCache.empty())
            {
                if (m_pT && m_pFun)
                {
                    (m_pT ->*m_pFun)(m_inputCache.front());
                }
                m_inputCache.pop();
            }
        }
        if (1)
        {
            CAutoLock _lock(&m_outputLock);
            while (!m_outputCache.empty())
            {
                if (m_pT && m_pFun)
                {
                    (m_pT ->*m_pFun)(m_outputCache.front());
                }
                m_outputCache.pop();
            }
        }
        
        m_bIsPopRunning = false;
    }
}

#endif // PING_PONG_CACHE_HPP