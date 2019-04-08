#include "BinaryMemPool.hpp"
#include "../file_opt/Log.hpp"

namespace zyb
{
    IMPLEMENT_SINGLETON(CBinaryMemPool):
        m_poolBuf32(1),
        m_poolBuf64(1),
        m_poolBuf128(1),
        m_poolBuf256(1),
        m_poolBuf512(1),
        m_poolBuf1k(1),
        m_poolBuf2k(1),
        m_poolBuf4k(1),
        m_poolBuf8k(1),
        m_poolBuf16k(1),
        m_poolBuf32k(1),
        m_poolBuf64k(1)
            
    {
        
    }
    
    CBinaryMemPool::~CBinaryMemPool()
    {
        
    }
    
    CBinaryBuf *CBinaryMemPool::ProvideBinaryBuf(unsigned int nSize)
    {
        CBinaryBuf *pRet = 0;
        for (;;)
        {
            if (nSize <= enumBinaryBuf_32)
            {
                pRet = m_poolBuf32.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_64)
            {
                pRet = m_poolBuf64.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_128)
            {
                pRet = m_poolBuf128.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_256)
            {
                pRet = m_poolBuf256.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_512)
            {
                pRet = m_poolBuf512.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_1k)
            {
                pRet = m_poolBuf1k.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_2k)
            {
                pRet = m_poolBuf2k.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_4k)
            {
                pRet = m_poolBuf4k.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_8k)
            {
                pRet = m_poolBuf8k.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_16k)
            {
                pRet = m_poolBuf16k.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_32k)
            {
                pRet = m_poolBuf32k.ProvideData();
                break;
            }
            if (nSize <= enumBinaryBuf_64k)
            {
                pRet = m_poolBuf64k.ProvideData();
                break;
            }
            LOG_ERROR("获取内存失败， 本次需要获取的内存大小为%d", nSize);
            break;
        }
        if (pRet)
        {
            pRet ->Clear();
        }
        return pRet;
    }
    
    bool CBinaryMemPool::DestroyBinaryBuf(CBinaryBuf *pBinaryBuf)
    {
        bool bRet = true;
        for (;;)
        {
            if (!pBinaryBuf)
            {
                LOG_ERROR("归还内存失败， 本次归还的内存地址为空");
                break;
            }
            switch (pBinaryBuf ->GetBinaryBufType())
            {
                case enumBinaryBuf_unknow:
                {
                    LOG_ERROR("归还内存失败， 本次归还的内存为 enumBinaryBuf_unknow");
                    break;
                }
                case enumBinaryBuf_32:
                {
                    CBinaryBuf32 *pTemp = dynamic_cast<CBinaryBuf32 *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf32* 类型失败"); 
                        break;
                    }
                    bRet = m_poolBuf32.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_64:
                {
                    CBinaryBuf64 *pTemp = dynamic_cast<CBinaryBuf64 *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf64* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf64.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_128:
                {
                    CBinaryBuf128 *pTemp = dynamic_cast<CBinaryBuf128 *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf128* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf128.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_256:
                {
                    CBinaryBuf256 *pTemp = dynamic_cast<CBinaryBuf256 *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf256* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf256.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_512:
                {
                    CBinaryBuf512 *pTemp = dynamic_cast<CBinaryBuf512 *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf512* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf512.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_1k:
                {
                    CBinaryBuf1k *pTemp = dynamic_cast<CBinaryBuf1k *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf1k* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf1k.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_2k:
                {
                    CBinaryBuf2k *pTemp = dynamic_cast<CBinaryBuf2k *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf2k* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf2k.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_4k:
                {
                    CBinaryBuf4k *pTemp = dynamic_cast<CBinaryBuf4k *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf4k* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf4k.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_8k:
                {
                    CBinaryBuf8k *pTemp = dynamic_cast<CBinaryBuf8k *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf8k* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf8k.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_16k:
                {
                    CBinaryBuf16k *pTemp = dynamic_cast<CBinaryBuf16k *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf16k* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf16k.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_32k:
                {
                    CBinaryBuf32k *pTemp = dynamic_cast<CBinaryBuf32k *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf32k* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf32k.DestroyData(pTemp);
                    break;
                }
                case enumBinaryBuf_64k:
                {
                    CBinaryBuf64k *pTemp = dynamic_cast<CBinaryBuf64k *>(pBinaryBuf);
                    if (!pTemp)
                    {
                        LOG_ERROR("转换为 CBinaryBuf64k* 类型失败");
                        break;
                    }
                    bRet = m_poolBuf64k.DestroyData(pTemp);
                    break;
                }
                default:
                {
                    LOG_ERROR("归还内存失败， 本次归还的内存为 %lu", pBinaryBuf ->GetBinaryBufType());
                    break;
                }
            }
            
            if (!bRet)
            {
                LOG_ERROR("归还内存失败， 本次归还的内存为 %lu", (long)pBinaryBuf);
            }
            
            break;
        }
        return bRet;
    }
}