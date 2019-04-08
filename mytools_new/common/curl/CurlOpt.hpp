#ifndef CURLOPT_HPP
#define CURLOPT_HPP
#include <curl/curl.h>
#include <string.h>

#define CURL_RECV_BUF_SIZE (1024 * 64)

class CCurlOpt
{
public:
    CCurlOpt();
    ~CCurlOpt();
    
    static bool CurlGlobalInit();
    static void CurlGlobalCleanup();
    
    const char *GetUrl(const char *pUrl);
    const char *PostUrl(const char *pUrl, const char *pPostBuf);
    
    // callback
    bool SetBuf(void *pPtr, int nSize);
    
private:
    static bool m_bGlobalInit;
    CURL * m_pCurl;
    char   m_szBuf[CURL_RECV_BUF_SIZE];
    int    m_nBufSize;
};


#endif // CURLOPT_HPP