#include "CurlOpt.hpp"

#define CURL_CONNECT_TIMEOUT_SECOND (5)
#define CURL_RECV_TIMEOUT_SECOND    (5)

bool CCurlOpt::m_bGlobalInit = false;

CCurlOpt::CCurlOpt()
{
    m_pCurl = 0;
    m_nBufSize = 0;
}

CCurlOpt::~CCurlOpt()
{
    if (m_pCurl)
    {
        curl_easy_cleanup(m_pCurl);
        m_pCurl = 0;
    }
}

bool CCurlOpt::CurlGlobalInit()
{
    if (m_bGlobalInit)
    {
        return true;
    }
    if (CURLE_OK != curl_global_init(CURL_GLOBAL_ALL))
    {
        return false;
    }
    
    return true;
}

void CCurlOpt::CurlGlobalCleanup()
{
    curl_global_cleanup();
    m_bGlobalInit = false;
}

 size_t write_data( void *ptr, size_t size, size_t nmemb, void *stream)
{
    int nCopy = size * nmemb;
    CCurlOpt *pThis = (CCurlOpt *)stream;
    if (0 == pThis)
    {
        return nCopy;
    }
    
    pThis ->SetBuf(ptr, nCopy);
    
    return nCopy;
}

const char *CCurlOpt::GetUrl(const char *pUrl)
{
    char *pRet = 0;
    for (;;)
    {
        if (!pUrl || 0 == pUrl[0])
        {
            break;
        }
        if (m_pCurl)
        {
            curl_easy_cleanup(m_pCurl);
            m_pCurl = 0;
        }
        
        m_pCurl = curl_easy_init();
        if (!m_pCurl)
        {
            break;
        }
        
        m_nBufSize = 0;
        memset(m_szBuf, 0, CURL_RECV_BUF_SIZE);
        // set timeout
        curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, CURL_CONNECT_TIMEOUT_SECOND);
        curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, CURL_RECV_TIMEOUT_SECOND);
        curl_easy_setopt(m_pCurl, CURLOPT_URL, pUrl);
        curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(m_pCurl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)this);
        
        CURLcode ret = curl_easy_perform(m_pCurl);
        if (CURLE_OK == ret)
        {
            pRet = m_szBuf;
        }
        
        curl_easy_cleanup(m_pCurl);
        m_pCurl = 0;
        break;
    }
    return pRet;
}

const char *CCurlOpt::PostUrl(const char *pUrl, const char *pPostBuf)
{
    char *pRet = 0;
    for (;;)
    {
        if (!pUrl || 0 == pUrl[0] || !pPostBuf || !pPostBuf[0])
        {
            break;
        }
        if (m_pCurl)
        {
            curl_easy_cleanup(m_pCurl);
            m_pCurl = 0;
        }
        
        m_pCurl = curl_easy_init();
        if (!m_pCurl)
        {
            break;
        }
        
        m_nBufSize = 0;
        memset(m_szBuf, 0, CURL_RECV_BUF_SIZE);
        curl_easy_setopt(m_pCurl, CURLOPT_CONNECTTIMEOUT, CURL_CONNECT_TIMEOUT_SECOND);
        curl_easy_setopt(m_pCurl, CURLOPT_TIMEOUT, CURL_RECV_TIMEOUT_SECOND);
        curl_easy_setopt(m_pCurl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(m_pCurl, CURLOPT_URL, pUrl);
        curl_easy_setopt(m_pCurl, CURLOPT_POST, 1);
        curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, pPostBuf);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, (void *)this);
        
        CURLcode ret = curl_easy_perform(m_pCurl);
        if (CURLE_OK == ret)
        {
            pRet = m_szBuf;
        }
        
        curl_easy_cleanup(m_pCurl);
        m_pCurl = 0;
        
        break;
    }
    return pRet;
}

bool CCurlOpt::SetBuf(void *pPtr, int nSize)
{
    bool bRet = false;
    for (;;)
    {
        if (!pPtr || 0 >= nSize)
        {
            break;
        }
        
        if (CURL_RECV_BUF_SIZE - m_nBufSize >= nSize)
        {
            memcpy(&m_szBuf[m_nBufSize], pPtr, nSize);
            m_nBufSize += nSize;
        }
        else
        {
            break;
        }

        bRet = true;
        break;
    }
    return bRet;
}

