#include "BinaryBuf.hpp"
#include <string.h>
#include "../file_opt/Log.hpp"

namespace zyb
{
    CBinaryBuf::CBinaryBuf()
    {
        m_pData = 0;
        m_emBinaryBufType = enumBinaryBuf_unknow;
        m_unValidStartIndex = 0;
        m_unValidEndIndex = 0;
    }
    
    bool CBinaryBuf::CopyBuf(const char *pData, unsigned int nDataSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!pData || nDataSize > m_emBinaryBufType)
            {
                LOG_ERROR("参数错误");
                break;
            }
            memcpy(m_pData, pData, nDataSize);
            m_unValidStartIndex = 0;
            m_unValidEndIndex = nDataSize;
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CBinaryBuf::AppendBuf(const char *pData, unsigned int nDataSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (!pData || nDataSize > (m_emBinaryBufType - m_unValidEndIndex))
            {
                LOG_ERROR("参数错误");
                break;
            }
            memcpy(m_pData + m_unValidEndIndex, pData, nDataSize);
            m_unValidEndIndex += nDataSize;
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CBinaryBuf::Rearrange()
    {
        bool bRet = false;
        for (;;)
        {
            int nValidDataSize = m_unValidEndIndex - m_unValidStartIndex;
            if (0 > nValidDataSize)
            {
                LOG_ERROR("memory 错误 startIndex=%d endIndex=%d", m_unValidStartIndex, m_unValidEndIndex);
                break;
            }
            if (0 == nValidDataSize)
            {
                m_unValidStartIndex = 0;
                m_unValidEndIndex = 0;
                bRet = true;
                break;
            }
            if (0 == m_unValidStartIndex)
            {
                bRet = true;
                break;
            }
            
            char szTemp[enumBinaryBuf_64k];
            memcpy(szTemp, &m_pData[m_unValidStartIndex], nValidDataSize);
            memcpy(m_pData, szTemp, nValidDataSize);
            
            m_unValidStartIndex = 0;
            m_unValidEndIndex = nValidDataSize;
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CBinaryBuf::Seek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (0 == nSeek)
            {
                bRet = true;
                break;
            }
            int nValidDataSize = m_unValidEndIndex - m_unValidStartIndex;
            
            int nSeekTemp = nSeek;
            if (0 > nSeek)
            {
                nSeekTemp = 0 - nSeekTemp;
            }
            
            if (nSeekTemp > nValidDataSize)
            {
                LOG_ERROR("数据长度不够");
                break;
            }
            else if (nSeekTemp == nValidDataSize)
            {
                m_unValidStartIndex = 0;
                m_unValidEndIndex   = 0;
            }
            else
            {
                0 > nSeek ? m_unValidEndIndex -= nSeekTemp : m_unValidStartIndex += nSeekTemp;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    const char *CBinaryBuf::GetBinaryBuf(unsigned int &nValidBufSize)
    {
        const char *pRet = 0;
        for (;;)
        {
            int nValidDataSizeTemp = m_unValidEndIndex - m_unValidStartIndex;
            if (0 > nValidDataSizeTemp)
            {
                LOG_ERROR("下标错误 m_unValidEndIndex=%u m_unValidStartIndex=%u", m_unValidEndIndex, m_unValidStartIndex);
                break;
            }
            if (0 == nValidDataSizeTemp)
            {
                pRet = m_pData;
                nValidBufSize = 0;
                break;
            }
            
            nValidBufSize = nValidDataSizeTemp;
            pRet = &m_pData[m_unValidStartIndex];
            break;
        }
        return pRet;
    }
    
    void CBinaryBuf::Clear()
    {
        m_unValidStartIndex = 0;
        m_unValidEndIndex = 0;
    }
    
    ENUM_BINARY_BUF_TYPE CBinaryBuf::GetBinaryBufType() const
    {
        return m_emBinaryBufType;
    }
    
    int CBinaryBuf::GetSurplusSize() const
    {
        return m_emBinaryBufType - m_unValidEndIndex;
    }
    
    bool CBinaryBuf::IsEmpty() const
    {
        return m_unValidEndIndex <= m_unValidStartIndex;
    }

    bool CBinaryBuf::RearrangeSeek(char *pBuf, unsigned int nBufSize)
    {
        bool bRet = false;
        for (;;)
        {
            if (m_unValidEndIndex <= m_unValidStartIndex)
            {
                m_unValidEndIndex = 0;
                m_unValidStartIndex = 0;
                bRet = true;
                break;
            }
            memcpy(pBuf, &m_pData[m_unValidStartIndex], m_unValidEndIndex - m_unValidStartIndex);
            bRet = CopyBuf(pBuf, m_unValidEndIndex - m_unValidStartIndex);
            break;
        }
        return bRet;
    }

    // 32 bit
    CBinaryBuf32::CBinaryBuf32()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_32;
    }
    
    bool CBinaryBuf32::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_32];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_32);
            break;
        }
        return bRet;
    }
    
   
    // 64 bit
    CBinaryBuf64::CBinaryBuf64()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_64;
    }
    
    bool CBinaryBuf64::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_64];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_64);
            break;
        }
        return bRet;
    }
    
    // 128 bit
    CBinaryBuf128::CBinaryBuf128()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_128;
    }
    
    bool CBinaryBuf128::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_128];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_128);
            break;
        }
        return bRet;
    }
    
    // 256 bit
    CBinaryBuf256::CBinaryBuf256()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_256;
    }
    
    bool CBinaryBuf256::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_256];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_256);
            break;
        }
        return bRet;
    }
    
    // 512 bit
    CBinaryBuf512::CBinaryBuf512()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_512;
    }
    
    bool CBinaryBuf512::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_512];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_512);
            break;
        }
        return bRet;
    }
    
    // 1k bit
    CBinaryBuf1k::CBinaryBuf1k()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_1k;
    }
    
    bool CBinaryBuf1k::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_1k];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_1k);
            break;
        }
        return bRet;
    }
    
    // 2k bit
    CBinaryBuf2k::CBinaryBuf2k()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_2k;
    }
    
    bool CBinaryBuf2k::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_2k];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_2k);
            break;
        }
        return bRet;
    }
    
    // 4k bit
    CBinaryBuf4k::CBinaryBuf4k()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_4k;
    }
    
    bool CBinaryBuf4k::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_4k];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_4k);
            break;
        }
        return bRet;
    }
    
    // 8k bit
    CBinaryBuf8k::CBinaryBuf8k()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_8k;
    }
    
    bool CBinaryBuf8k::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_8k];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_8k);
            break;
        }
        return bRet;
    }
    
    // 16k bit
    CBinaryBuf16k::CBinaryBuf16k()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_16k;
    }
    
    bool CBinaryBuf16k::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_16k];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_16k);
            break;
        }
        return bRet;
    }
    
    // 32k bit
    CBinaryBuf32k::CBinaryBuf32k()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_32k;
    }
    
    bool CBinaryBuf32k::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_32k];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_32k);
            break;
        }
        return bRet;
    }
    
    // 64k bit
    CBinaryBuf64k::CBinaryBuf64k()
    {
        m_pData = m_szData;
        m_emBinaryBufType = enumBinaryBuf_64k;
    }
    
    bool CBinaryBuf64k::RearrangeSeek(int nSeek)
    {
        bool bRet = false;
        for (;;)
        {
            if (!Seek(nSeek))
            {
                break;
            }
            
            char szTemp[enumBinaryBuf_64k];
            bRet = CBinaryBuf::RearrangeSeek(szTemp, enumBinaryBuf_64k);
            break;
        }
        return bRet;
    }
    
    
}