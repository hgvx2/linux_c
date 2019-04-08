#ifndef __BINARY_MEM_POOL_HPP
#define __BINARY_MEM_POOL_HPP

#include "BinaryBuf.hpp"
#include "../design_patterns/Singleton.hpp"
#include "../memory_pool/MemoryPool.hpp"


namespace zyb
{
    class CBinaryMemPool
    {
        DECLARE_SINGLETON(CBinaryMemPool)
    public:
        ~CBinaryMemPool();
        
        CBinaryBuf *ProvideBinaryBuf(unsigned int nSize);
        bool DestroyBinaryBuf(CBinaryBuf *pBinaryBuf);
        
    private:
        CMemoryPool0<CBinaryBuf32>  m_poolBuf32;
        CMemoryPool0<CBinaryBuf64>  m_poolBuf64;
        CMemoryPool0<CBinaryBuf128> m_poolBuf128;
        CMemoryPool0<CBinaryBuf256> m_poolBuf256;
        CMemoryPool0<CBinaryBuf512> m_poolBuf512;
        CMemoryPool0<CBinaryBuf1k>  m_poolBuf1k;
        CMemoryPool0<CBinaryBuf2k>  m_poolBuf2k;
        CMemoryPool0<CBinaryBuf4k>  m_poolBuf4k;
        CMemoryPool0<CBinaryBuf8k>  m_poolBuf8k;
        CMemoryPool0<CBinaryBuf16k> m_poolBuf16k;
        CMemoryPool0<CBinaryBuf32k> m_poolBuf32k;
        CMemoryPool0<CBinaryBuf64k> m_poolBuf64k;
    };
}


#endif // __BINARY_MEM_POOL_HPP