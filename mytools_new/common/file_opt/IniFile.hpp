#ifndef INIFILE_HPP
#define INIFILE_HPP

#include <stdio.h>
#include <map>
#include <list>
#include <string>

/*
 * 注意：ini配置文件, 每行的最大长度不得超过4096个字节,否则解析失败
 */

namespace zyb 
{
     
    enum emLineType
    {
        em_UnknowLine = 0,  // 未知行
        em_SpaceLine,       // 空行
        em_NoteLine,        // 注释行
        em_SectionLine,     // 节点行
        em_KeyValueLine     // 键值行
    };

    struct _tagIniLineNode
    {
        std::string strKey;
        std::string strValue;
        std::string strNote;
        emLineType  lineType;
        int nIndex;
        
        void InitIniLineNode()
        {
            this ->strKey.clear();
            this ->strValue.clear();
            this ->strNote.clear();
            this ->lineType = em_UnknowLine;
            this ->nIndex = -1;
        }
    };
    
    typedef std::map<const std::string, _tagIniLineNode *> mapLineNode;
    
    struct _tagIniSectionNode
    {
        std::string strSection;
        std::string strNote;
        int nIndex;
        
        mapLineNode mapSectionAllLineNode;
        
        void InitIniSectionNode()
        {
            this ->strSection.clear();
            this ->strNote.clear();
            this ->nIndex = -1;
            this ->mapSectionAllLineNode.clear();
        }
    };
    
    typedef std::map<const std::string, _tagIniSectionNode *> mapSectionNode;

    class CIniFile
    {
    public:
        CIniFile();
        ~CIniFile();

        // 解析INI文件
        bool AnalysisIniFile(const char *pIniFile);
        
        // 
        bool GetPrivateProfileString(const char *pSection, const char *pKey, char *pRecvBuf, int nRecvBufSize, const char *pDefaultValue="");
        
        //
        bool GetPrivateProfileInt(const char *pSection, const char *pKey, int &nValue, int nDefaultValue = 0);
        
        //
        bool WriteKeyValueString(const char *pSection, const char *pKey, const char *pValue, const char *pNote = NULL);
        
        //
        bool WriteKeyValueInt(const char *pSection, const char *pKey, int nValue, const char *pNote = NULL);
        
        //
        bool SaveIniFile();
        
        //
        void ClearAnalysisIniFile();
        
    private:
        
         //
        bool WriteSection(const char *pSection, const char *pNote = NULL);
        
        bool AnalysisIniFile();
        
        bool AnalysisLine(char *pBuf);
        
        _tagIniLineNode *AnalysisLineToNode(char *pBuf);
        
        
    private:
        FILE *m_pFile;
        bool m_bChanged;
        bool m_bInit;
        
        std::string m_iniFilePath;
        mapSectionNode m_mapSection;
        _tagIniSectionNode * m_pCurSection;
    };
}

#endif // INIFILE_HPP