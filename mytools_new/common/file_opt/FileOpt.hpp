#ifndef FILE_OPT_HPP
#define FILE_OPT_HPP

namespace zyb
{
#define FILE_OPT_MAX_PATH (260)  // 
    
    class CFileOpt
    {
    public:
        // 获取当前进程的路径
        static  bool GetCurrentPath(char szPath[FILE_OPT_MAX_PATH]);
        
        // 文件是否存在
        static bool IsFileExist(const char *pFile);
        
        // 是否为文件夹
        static bool IsDirectory(const char *pDirPath);
        
        //文件夹是否存在
        static bool IsDirectoryExist(const char *pDirPath);
        
        // 创建目录
        static bool CreateDirectory(const char *pDirPath);
    };
}

#endif // FILE_OPT_HPP