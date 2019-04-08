#include "FileOpt.hpp"

#include <string>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>


namespace zyb
{
    // 获取当前进程的路径
    bool CFileOpt::GetCurrentPath(char szPath[FILE_OPT_MAX_PATH])
    {
        bool bRet = false;
        for (;;)
        {
            char szPidFile[64] = {0};
            snprintf(szPidFile, 64, "/proc/%d/cmdline", getpid());
            
            int nFd = open(szPidFile, O_RDONLY, 0);
            int nReadSize = read(nFd, szPath, FILE_OPT_MAX_PATH);
            close(nFd);
            if (0 >= nReadSize)
            {
                break;
            }
            
            int nIndex = nReadSize - 1;
            char *p = &szPath[nIndex];
            do
            {
                p --;
                
            }while(0 < nIndex && '/' != *p);
            
            if (0 >= nIndex)
            {
                break;
            }
            *p = 0;
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    // 文件是否存在
    bool CFileOpt::IsFileExist(const char *pFile)
    {
        bool bRet = false;
        for (;;)
        {
            if (0 == pFile || 0 == pFile[0])
            {
                break;
            }
            if (CFileOpt::IsDirectory(pFile))
            {
                break;
            }
            if (0 != access(pFile, F_OK))
            {
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
    
    // 是否为文件夹
    bool CFileOpt::IsDirectory(const char *pDirPath)
    {
        bool bRet = false;
        for (;;)
        {
            if (0 == pDirPath || 0 == pDirPath[0])
            {
                break;
            }
            struct stat st;
            stat(pDirPath,&st);
            if (!S_ISDIR(st.st_mode))
            {
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    // 文件夹是否存在
    bool CFileOpt::IsDirectoryExist(const char *pDirPath)
    {
        bool bRet = false;
        for (;;)
        {
            bRet = IsDirectory(pDirPath);
            break;
        }
        return bRet;
    }
    
    // 创建目录
    bool CFileOpt::CreateDirectory(const char *pDirPath)
    {
        bool bRet = false;
        for (;;)
        {
            if (0 == pDirPath || 0 == pDirPath[0])
            {
                break;
            }
            if (CFileOpt::IsDirectoryExist(pDirPath))
            {
                bRet = true;
                break;
            }
            
            int nSubStr = 0;
            std::string strArr[100];
            
            char szPath[FILE_OPT_MAX_PATH] = {0};
            int nLen = snprintf(szPath, FILE_OPT_MAX_PATH, "%s", pDirPath);
            if (0 >= nLen)
            {
                break;
            }
           
            char *p = &szPath[nLen];
            if ('/' == *p)
            {
                *p = 0;
                p --;
                nLen --;
            }
            strArr[nSubStr ++] = szPath;
            do
            {
                if ('/' == *p)
                {
                    *p = 0;
                    if (CFileOpt::IsDirectoryExist(szPath))
                    {
                        break;
                    }
                    strArr[nSubStr ++] = szPath;
                }
                
                p --;
                nLen --;        
                
            }while(0 < nLen);
                
            bool bComplate = true;
            for (int i = nSubStr - 1; i >= 0; i --)
            {
                if (mkdir(strArr[i].c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO))
                {
                    bComplate = false;
                    break;
                }
            }
            
            if (!bComplate)
            {
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
}