#ifndef MEMORY_POOL_HPP
#define MEMORY_POOL_HPP

#include "../lock/Lock.hpp"
#include <list>
#include <map>

namespace zyb 
{
    template<typename T>
    class CMemoryPool
    {
    public:
        CMemoryPool(){};
        virtual ~CMemoryPool();

        T *ProvideData();
        bool DestroyData(T *pData);
    private:
        virtual T *NewObject() = 0;

    protected:
        std::list<T *> m_listFreeQueue;
        std::map<long, T *> m_mapUseQueue;
        CLock m_lock;
        bool m_bInit;
    };
    
    template<typename T>
    CMemoryPool<T>::~CMemoryPool() 
    {
        CAutoLock _autoLock(&m_lock);
        typename std::list<T *>::iterator listIt = m_listFreeQueue.begin();
        for (; listIt != m_listFreeQueue.end(); listIt++) 
        {
            delete (*listIt);
        }
        m_listFreeQueue.clear();

        typename std::map<long, T *>::iterator mapIt = m_mapUseQueue.begin();
        for (; mapIt != m_mapUseQueue.end(); mapIt++) 
        {
            delete mapIt ->second;
        }
        m_mapUseQueue.clear();

    }

    template<typename T>
    T *CMemoryPool<T>::ProvideData() 
    {
        T *pData = 0;
        for (;;) 
        {
            if (!m_bInit)
            {
                break;
            }
            CAutoLock _autolock(&m_lock);
            if (!m_listFreeQueue.empty()) 
            {
                pData = *m_listFreeQueue.begin();
                m_listFreeQueue.pop_front();
                m_mapUseQueue.insert(std::pair<long, T *>((long) pData, pData));
                break;
            }

            T *pTemp = NewObject();
            if (!pTemp) 
            {
                break;
            }
            pData = pTemp;
            m_mapUseQueue.insert(std::pair<long, T *>((long) pData, pData));
            break;
        }
        return pData;
    }

    template<typename T>
    bool CMemoryPool<T>::DestroyData(T *pData) 
    {
        bool bRet = false;
        for (;;) 
        {
            if (!m_bInit)
            {
                break;
            }
            if (pData) 
            {
                CAutoLock _autoLock(&m_lock);
                typename std::map<long, T *>::iterator mapIt = m_mapUseQueue.find((long) pData);
                if (mapIt == m_mapUseQueue.end()) 
                {
                    break;
                }
                m_listFreeQueue.push_back(mapIt ->second);
                m_mapUseQueue.erase(mapIt);
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    
    // 无参数内存池
    template<typename T>
    class CMemoryPool0 : public CMemoryPool<T>
    {
        public:
            CMemoryPool0(unsigned int nMin);
            ~CMemoryPool0(){}
        private:
            T *NewObject();
    };
    
    template<typename T>
    CMemoryPool0<T>::CMemoryPool0(unsigned int nMin)
    {
        this ->m_listFreeQueue.clear();
        this ->m_mapUseQueue.clear();
        this ->m_bInit = true;

        CAutoLock _autoLock(&this ->m_lock);
        for (int i = 0; i < nMin; i ++) 
        {
            T *pTemp = new T;
            if (pTemp) 
            {
                this ->m_listFreeQueue.push_back(pTemp);
            }
            else
            {
                this ->m_bInit = false;
                break;
            }
        }
    }
    
    template<typename T>
    T *CMemoryPool0<T>::NewObject()
    {
        T *pRet = 0;
        for (;;)
        {
            if (!this ->m_bInit)
            {
                break;
            }
            pRet = new T;
            break;
        }
        return pRet;
    }
    
    // 1个参数内存池
    template<typename T, typename P>
    class CMemoryPool1 : public CMemoryPool<T>
    {
        public:
            CMemoryPool1(unsigned int nMin, P p1);
            virtual ~CMemoryPool1(){};
        private:    
            T *NewObject();
            
        private:
            P m_p;
    };
    
    template<typename T, typename P>
    CMemoryPool1<T, P>::CMemoryPool1(unsigned int nMin, P p1)
    {
        this ->m_listFreeQueue.clear();
        this ->m_mapUseQueue.clear();
        this ->m_bInit = true;
        m_p = p1;

        CAutoLock _autoLock(&this ->m_lock);
        for (int i = 0; i < nMin; i ++) 
        {
            T *pTemp = new T(p1);
            if (pTemp) 
            {
                this ->m_listFreeQueue.push_back(pTemp);
            }
            else
            {
                this ->m_bInit = false;
                break;
            }
        }
    }
    
    template<typename T, typename P>
    T *CMemoryPool1<T, P>::NewObject()
    {
        T *pRet = 0;
        for (;;)
        {
            if (!this ->m_bInit)
            {
                break;
            }
            pRet = new T(m_p);
            break;
        }
        return pRet;
    }
    
    // 2个参数内存池

    template<typename T, typename P, typename Q>
    class CMemoryPool2 : public CMemoryPool<T>
    {
        public:
            CMemoryPool2(unsigned int nMin, P p1, Q q2);
            virtual ~CMemoryPool2(){};
        private:     
            T *NewObject();
            
        private:
            P m_p;
            Q m_q;
    };
    
    template<typename T, typename P, typename Q>
    CMemoryPool2<T, P, Q>::CMemoryPool2(unsigned int nMin, P p1, Q q2)
    {
        this ->m_listFreeQueue.clear();
        this ->m_mapUseQueue.clear();
        this ->m_bInit = true;
        m_p = p1;
        m_q = q2;

        CAutoLock _autoLock(&this ->m_lock);
        for (int i = 0; i < nMin; i ++) 
        {
            T *pTemp = new T(p1, q2);
            if (pTemp) 
            {
                this ->m_listFreeQueue.push_back(pTemp);
            }
            else
            {
                this ->m_bInit = false;
                break;
            }
        }
    }
    
    template<typename T, typename P, typename Q>
    T *CMemoryPool2<T, P, Q>::NewObject()
    {
        T *pRet = 0;
        for (;;)
        {
            if (!this ->m_bInit)
            {
                break;
            }
            pRet = new T(m_p, m_q);
            break;
        }
        return pRet;
    }
    
    // 3个参数内存池
    template<typename T, typename P, typename Q, typename R>
    class CMemoryPool3 : public CMemoryPool<T>
    {
        public:
            CMemoryPool3(unsigned int nMin, P p1, Q q2, R r3);
            virtual ~CMemoryPool3(){};
        private:     
            T *NewObject();
            
        private:
            P m_p;
            Q m_q;
            R m_r;
    };
    
    template<typename T, typename P, typename Q, typename R>
    CMemoryPool3<T, P, Q, R>::CMemoryPool3(unsigned int nMin, P p1, Q q2, R r3)
    {
        this ->m_listFreeQueue.clear();
        this ->m_mapUseQueue.clear();
        this ->m_bInit = true;
        m_p = p1;
        m_q = q2;
        m_r = r3;

        CAutoLock _autoLock(&this ->m_lock);
        for (int i = 0; i < nMin; i ++) 
        {
            T *pTemp = new T(p1, q2, r3);
            if (pTemp) 
            {
                this ->m_listFreeQueue.push_back(pTemp);
            }
            else
            {
                this ->m_bInit = false;
                break;
            }
        }
    }
    
    template<typename T, typename P, typename Q, typename R>
    T *CMemoryPool3<T, P, Q, R>::NewObject()
    {
        T *pRet = 0;
        for (;;)
        {
            if (!this ->m_bInit)
            {
                break;
            }
            pRet = new T(m_p, m_q, m_r);
            break;
        }
        return pRet;
    }
}

#endif // MEMORY_POOL_HPP
