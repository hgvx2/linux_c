#ifndef LOCK_HPP
#define LOCK_HPP
#include<pthread.h>

namespace zyb 
{
    class CLock 
    {
    public:
        CLock();
        ~CLock();

        void Lock();
        void UnLock();
        pthread_mutex_t *GetMutex();

    private:
        pthread_mutex_t m_lock;
    };

    class CAutoLock 
    {
    public:
        CAutoLock(CLock *pLock);
        ~CAutoLock();

    private:
        CLock *m_pLock;
    };
    
    class CShareRWLock
    {
    public:
        CShareRWLock();
	~CShareRWLock();
        
        void LockRead();
	void LockWrite();
	void UnLock();
        
    private:
        pthread_rwlock_t m_rwlock;
    };
    
    class CAutoShareReadLock
    {
    public:
	enum LockType
	{
            emLT_ReadLock = 0,
            emLT_WriteLock
	};

	CAutoShareReadLock(CShareRWLock *pLock, CAutoShareReadLock::LockType type);

	~CAutoShareReadLock();

    private:
	CShareRWLock *m_pLock;
	CAutoShareReadLock::LockType m_type;
    };
}

#endif // LOCK_HPP
