#include "Lock.hpp"

namespace zyb 
{

    CLock::CLock() 
    {
        pthread_mutex_init(&m_lock, NULL);
    }

    CLock::~CLock() 
    {
        pthread_mutex_destroy(&m_lock);
    }

    pthread_mutex_t *CLock::GetMutex() 
    {
        return &m_lock;
    }

    void CLock::Lock() 
    {
        pthread_mutex_lock(&m_lock);
    }

    void CLock::UnLock() 
    {
        pthread_mutex_unlock(&m_lock);
    }

    CAutoLock::CAutoLock(CLock *pLock) 
    {
        m_pLock = pLock;
        if (0 != m_pLock) 
        {
            m_pLock ->Lock();
        }
    }

    CAutoLock::~CAutoLock() 
    {
        if (0 != m_pLock) 
        {
            m_pLock ->UnLock();
        }
    }
    
    CShareRWLock::CShareRWLock()
    {
        pthread_rwlock_init(&m_rwlock, NULL);
    }
    CShareRWLock::~CShareRWLock()
    {
        pthread_rwlock_destroy(&m_rwlock);
    }
    void CShareRWLock::LockRead()
    {
        pthread_rwlock_rdlock(&m_rwlock);//请求读锁
    }
    void CShareRWLock::LockWrite()
    {
        pthread_rwlock_wrlock(&m_rwlock);//请求写锁
    }
    void CShareRWLock::UnLock()
    {
        pthread_rwlock_unlock(&m_rwlock);//解锁
    }
    
    CAutoShareReadLock::CAutoShareReadLock(CShareRWLock *pLock, CAutoShareReadLock::LockType type)
    {
	m_pLock = pLock;
	m_type  = type;

	if (emLT_ReadLock == m_type)
	{
            pLock ->LockRead();
	}
	if (emLT_WriteLock == m_type)
	{
            pLock ->LockWrite();
	}
    }

    CAutoShareReadLock::~CAutoShareReadLock()
    {
	m_pLock ->UnLock();
    }
}