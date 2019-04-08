#include "IniFile.hpp"
#include "FileOpt.hpp"

#include <string.h>
#include <stdlib.h>
namespace zyb
{
    
    const char *g_pUnknowHead = "*--unknow_head_***";
    const char *g_pSpace      = "*--space_***";
    const char *g_pNote       = "*--note_***";
    
    CIniFile::CIniFile()
    {
        m_pFile    = 0;
        m_mapSection.clear();
        m_iniFilePath.clear();
        m_bChanged = false;
        m_bInit    = false;
        m_pCurSection = 0;
                
    }
    
    CIniFile::~CIniFile()
    {
        ClearAnalysisIniFile();
    }
    
    // 解析INI文件
    bool CIniFile::AnalysisIniFile(const char *pIniFile)
    {
        bool bRet = false;
        for (;;)
        {
            if (m_bInit)
            {
                bRet = true;
                break;
            }
            if (!pIniFile || 0 == pIniFile[0])
            {
                break;
            }
            m_iniFilePath = pIniFile;
            if (!CFileOpt::IsFileExist(pIniFile))
            {
                break;
            }
            m_pFile = fopen(pIniFile, "r");
            if (!m_pFile) 
            {
                break;
            }
            
            bRet = AnalysisIniFile();
            
            fclose(m_pFile);
            m_pFile = 0;
            break;
        }
        return bRet;
    }
    
    //
    bool CIniFile::GetPrivateProfileString(const char *pSection, const char *pKey, char *pRecvBuf, int nRecvBufSize, const char *pDefaultValue/*=""*/)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                break;
            }
            
            mapSectionNode::const_iterator mapcIt = m_mapSection.find(pSection);
            if (mapcIt == m_mapSection.end())
            {
                break;
            }
            if (!mapcIt ->second)
            {
                break;
            }
            
            mapLineNode::const_iterator mapcItLine = mapcIt ->second ->mapSectionAllLineNode.find(pKey);
            if (mapcItLine == mapcIt ->second ->mapSectionAllLineNode.end())
            {
                break;
            }
            if (!mapcItLine ->second)
            {
                break;
            }
            
            if (mapcItLine ->second->strValue.empty())
            {
                snprintf(pRecvBuf, nRecvBufSize, "%s", pDefaultValue);
            }
            else
            {
                snprintf(pRecvBuf, nRecvBufSize, "%s", mapcItLine ->second->strValue.c_str());
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CIniFile::GetPrivateProfileInt(const char *pSection, const char *pKey, int &nValue, int nDefaultValue/* = 0*/)
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                break;
            }
            
            mapSectionNode::const_iterator mapcIt = m_mapSection.find(pSection);
            if (mapcIt == m_mapSection.end())
            {
                break;
            }
            if (!mapcIt ->second)
            {
                break;
            }
            
            mapLineNode::const_iterator mapcItLine = mapcIt ->second ->mapSectionAllLineNode.find(pKey);
            if (mapcItLine == mapcIt ->second ->mapSectionAllLineNode.end())
            {
                break;
            }
            if (!mapcItLine ->second)
            {
                break;
            }
            
            if (mapcItLine ->second->strValue.empty())
            {
                nValue = nDefaultValue;
            }
            else
            {
                nValue = atoi(mapcItLine ->second->strValue.c_str());
            }
            
            
            bRet = true;
            break;
        }
        return bRet;
    }
  
    //
    bool CIniFile::WriteKeyValueString(const char *pSection, const char *pKey, const char *pValue, const char *pNote/* = NULL*/)
    {
        bool bRet = false;
        for (;;)
        {
            if (!pSection || !pKey || !pValue ||
                0 == pSection[0] || 0 == pKey[0])
            {
                break;
            }
            if (!m_bInit)
            {
                break;
            }
            
            if (!WriteSection(pSection))
            {
                break;
            }
            
            mapSectionNode::iterator mapIt = m_mapSection.find(pSection);
            if (mapIt == m_mapSection.end())
            {
                break;
            }
            
            mapLineNode::iterator mapItLine = mapIt ->second ->mapSectionAllLineNode.find(pKey);
            if (mapItLine != mapIt ->second->mapSectionAllLineNode.end())
            {
                mapItLine ->second->strValue = pValue;
                if (pNote && 0 != pNote[0])
                {
                    mapItLine ->second ->strNote = pNote;
                }
                bRet = true;
                break;
            }
            
            _tagIniLineNode *pLineNode = new _tagIniLineNode;
            if (!pLineNode)
            {
                break;
            }
            pLineNode ->InitIniLineNode();
            pLineNode ->nIndex = (int)mapIt ->second->mapSectionAllLineNode.size();
            pLineNode ->lineType = em_KeyValueLine;
            pLineNode ->strKey = pKey;
            pLineNode ->strValue = pValue;
            if (pNote && 0 != pNote[0])
            {
                pLineNode ->strNote = pNote;
            }
            
            std::pair<mapLineNode::iterator, bool> insertRet;
            insertRet = mapIt ->second ->mapSectionAllLineNode.insert(std::pair<const std::string, _tagIniLineNode *>(pKey, pLineNode));
            if (!insertRet.second)
            {
                delete pLineNode;
                pLineNode = 0;
                break;
            }
            bRet = true;
            break;
        }
        return bRet;
    }
        
    //
    bool CIniFile::WriteKeyValueInt(const char *pSection, const char *pKey, int nValue, const char *pNote/* = NULL*/)
    {
        bool bRet = false;
        for (;;)
        {
            char szValue[64] = {0};
            snprintf(szValue, 64, "%d", nValue);
            
            bRet = WriteKeyValueString(pSection, pKey, szValue, pNote);
            break;
        }
        return bRet;
    }
    
    bool CIniFile::SaveIniFile()
    {
        bool bRet = false;
        for (;;)
        {
            if (!m_bInit)
            {
                break;
            }
            
            m_pFile = fopen(m_iniFilePath.c_str(), "w");
            if (!m_pFile) 
            {
                break;
            }
            
            int nSectionSize = (int)m_mapSection.size();
            if (0 >= nSectionSize)
            {
                fclose(m_pFile);
                m_pFile = 0;
                bRet = true;
                break;
            }
            
            _tagIniSectionNode **pArrSection = new _tagIniSectionNode *[nSectionSize];
            mapSectionNode::const_iterator mapSecIt = m_mapSection.begin();
            while (mapSecIt != m_mapSection.end())
            {
                pArrSection[mapSecIt ->second->nIndex] = mapSecIt ->second;
                mapSecIt ++;
            }
            
            char szBufTemp[4096];
            int nW = 0;
            bool bWrite = true;
            for (int i = 0; i < nSectionSize; i ++)
            {
                if (!pArrSection[i] ->strSection.empty())
                {
                    if (!pArrSection[i] ->strNote.empty())
                    {
                        snprintf(szBufTemp, 4096, "[%s] ; %s\n", pArrSection[i] ->strSection.c_str(), pArrSection[i] ->strNote.c_str());
                    }
                    else
                    {
                        snprintf(szBufTemp, 4096, "[%s]\n", pArrSection[i] ->strSection.c_str());
                    }
                    
                    nW = fwrite(szBufTemp, strlen(szBufTemp), 1, m_pFile);
                    if (1 != bWrite)
                    {
                        bWrite = false;
                        break;
                    }
                }
                
                int nLineSize = (int)pArrSection[i] ->mapSectionAllLineNode.size();
                if (0 >= nLineSize)
                {
                    continue;
                }
                
                _tagIniLineNode **pArrLineNode = new _tagIniLineNode*[nLineSize];
                mapLineNode::const_iterator mapLineIt = pArrSection[i] ->mapSectionAllLineNode.begin();
                while (mapLineIt != pArrSection[i] ->mapSectionAllLineNode.end())
                {
                    pArrLineNode[mapLineIt ->second->nIndex] = mapLineIt ->second;
                    mapLineIt ++;
                }
                
                for (int j = 0; j < nLineSize; j ++)
                {
                    switch (pArrLineNode[j] ->lineType)
                    {
                        case em_UnknowLine:
                        {
                            bWrite = false;
                            break;
                        }
                        case em_SpaceLine:
                        {
                            nW = fwrite("\n", 1, 1, m_pFile);
                            if (1 != bWrite)
                            {
                                bWrite = false;
                                break;
                            }
                            break;
                        }
                        case em_NoteLine:
                        {
                            snprintf(szBufTemp, 4096, "; %s\n", pArrLineNode[j]->strNote.c_str());
                            nW = fwrite(szBufTemp, strlen(szBufTemp), 1, m_pFile);
                            if (1 != bWrite)
                            {
                                bWrite = false;
                                break;
                            }
                            break;
                        }
                        case em_SectionLine:
                        {
                            bWrite = false;
                            break;
                        }
                        case em_KeyValueLine:
                        {
                            if (pArrLineNode[j]->strNote.empty())
                            {
                                snprintf(szBufTemp, 4096, "%s=%s\n", pArrLineNode[j]->strKey.c_str(), pArrLineNode[j]->strValue.c_str());
                            }
                            else
                            {
                                snprintf(szBufTemp, 4096, "%s=%s ; %s\n", pArrLineNode[j]->strKey.c_str(), pArrLineNode[j]->strValue.c_str(), pArrLineNode[j]->strNote.c_str());
                            }
                            
                            nW = fwrite(szBufTemp, strlen(szBufTemp), 1, m_pFile);
                            if (1 != bWrite)
                            {
                                bWrite = false;
                                break;
                            }
                            break;
                        }
                        default:
                        {
                            bWrite = false;
                            break;
                        }
                    }
                    
                    if (!bWrite)
                    {
                        break;
                    }
                    
                }
                
                delete []pArrLineNode;
                pArrLineNode = 0;
                
                if (!bWrite)
                {
                    break;
                }
            }
            
            delete []pArrSection;
            pArrSection = 0;
            
            fclose(m_pFile);
            m_pFile = 0;
            if (!bWrite)
            {
                break;
            }
             
            bRet = true;
            break;
        }
        return bRet;
    }
    
    //

    void CIniFile::ClearAnalysisIniFile()
    {
        m_bChanged = false;
        m_bInit = false;
        m_iniFilePath.clear();
        if (m_pFile)
        {
            fclose(m_pFile);
            m_pFile = 0;
        }

        m_pCurSection = 0;
        
        mapSectionNode::iterator mapIt = m_mapSection.begin();
        while (mapIt != m_mapSection.end()) 
        {
            if (mapIt ->second) 
            {
                mapLineNode::iterator mapLineIt = mapIt ->second ->mapSectionAllLineNode.begin();
                while (mapLineIt != mapIt ->second ->mapSectionAllLineNode.end())
                {
                    if (mapLineIt ->second)
                    {
                        delete mapLineIt ->second;
                        mapLineIt ->second = 0;
                    }
                    mapLineIt ++;
                }
                mapIt ->second ->mapSectionAllLineNode.clear();
                
                delete mapIt ->second;
                mapIt ->second = 0;
            }
            mapIt++;
        }
        m_mapSection.clear();
    }
    
     //
    bool CIniFile::WriteSection(const char *pSection, const char *pNote/* = NULL*/)
    {
        bool bRet = false;
        for (;;)
        {
            if (!pSection || 0 == pSection[0])
            {
                break;
            }
            if (!m_bInit)
            {
                break;
            }
  
            mapSectionNode::const_iterator mapIt = m_mapSection.find(pSection);
            if (mapIt != m_mapSection.end())
            {
                if (pNote && 0 != pNote[0])
                {
                    mapIt ->second ->strNote = pNote;
                }
                bRet = true;
                break;
            }
            
            _tagIniSectionNode *pSectionNode = new _tagIniSectionNode;
            if (!pSectionNode)
            {
                break;
            }
            pSectionNode ->InitIniSectionNode();
            pSectionNode ->nIndex = (int)m_mapSection.size();
            pSectionNode ->strSection = pSection;
            if (pNote && 0 != pNote[0])
            {
                pSectionNode ->strNote = pNote;
            }
            
            std::pair<mapSectionNode::iterator, bool> insertRet;
            insertRet = m_mapSection.insert(std::pair<const std::string, _tagIniSectionNode *>(pSection, pSectionNode));
            
            if (!insertRet.second)
            {
                delete pSectionNode;
                pSectionNode = 0;
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CIniFile::AnalysisIniFile()
    {
        bool bRet = false;
        for (;;)
        {
            char szBuf[4096] = {0};
            bool bAnalysis = true;
            
            while(!feof(m_pFile))
            {
                if (!fgets(szBuf, 4096, m_pFile))
                {
                    break;
                }
                
                if (!AnalysisLine(szBuf))
                {
                    bAnalysis = false;
                    break;
                }
            }   
            
            if (!bAnalysis)
            {
                break;
            }
            
            m_bInit = true;
            bRet = true;
            break;
        }
        return bRet;
    }
    
    bool CIniFile::AnalysisLine(char *pBuf)
    {
        bool bRet = false;
        for (;;)
        {
            _tagIniLineNode *pNode = AnalysisLineToNode(pBuf);
            if (!pNode)
            {
                break;
            }
            
            bool bAnalysis = true;
            switch (pNode ->lineType)
            {
                case em_UnknowLine:
                {
                    bAnalysis = false;
                    break;
                }
                case em_SpaceLine:
                case em_NoteLine:
                {
                    if (!m_pCurSection)
                    {
                        m_pCurSection = new _tagIniSectionNode;
                        if (!m_pCurSection)
                        {
                            bAnalysis = false;
                            break;
                        }
                        m_pCurSection ->InitIniSectionNode();
                        m_pCurSection ->nIndex = (int)m_mapSection.size();
                        
                        std::pair<mapSectionNode::iterator, bool> mapRet;
                        mapRet = m_mapSection.insert(std::pair<const std::string, _tagIniSectionNode *>(g_pUnknowHead, m_pCurSection));
                        if (!mapRet.second)
                        {
                            delete m_pCurSection;
                            m_pCurSection = 0;
                            bAnalysis = false;
                            break;
                        }
                    }
                    
                    pNode ->nIndex = (int)m_pCurSection ->mapSectionAllLineNode.size();
                    
                    char szTemp[4096] = {0};
                    if (m_pCurSection ->strSection.empty())
                    {
                        snprintf(szTemp, 4096, "%s%s%d", g_pUnknowHead, em_SpaceLine == pNode ->lineType ? g_pSpace : g_pNote, pNode ->nIndex);
                    }
                    else
                    {
                        snprintf(szTemp, 4096, "%s%s%d", m_pCurSection ->strSection.c_str(), em_SpaceLine == pNode ->lineType ? g_pSpace : g_pNote, pNode ->nIndex);
                    }
                    
                    std::pair<mapLineNode::iterator, bool> mapInsertRet;
                    mapInsertRet = m_pCurSection ->mapSectionAllLineNode.insert(std::pair<const std::string, _tagIniLineNode *>(szTemp, pNode));
                    if (!mapInsertRet.second)
                    {
                        bAnalysis = false;
                        break;
                    }
                    
                    break;
                }
                case em_SectionLine:
                {
                    _tagIniSectionNode *pSectionNode = new _tagIniSectionNode;
                    if (!pSectionNode)
                    {
                        bAnalysis = false;
                        break;
                    }
                    pSectionNode ->nIndex     = (int)m_mapSection.size();
                    pSectionNode ->strSection = pNode ->strValue;
                    pSectionNode ->strNote    = pNode ->strNote;
                    
                    delete pNode;
                    pNode = 0;
                    
                    std::pair<mapSectionNode::iterator, bool> mapInsertRet;
                    mapInsertRet = m_mapSection.insert(std::pair<const std::string, _tagIniSectionNode *>(pSectionNode ->strSection, pSectionNode));
                    if (!mapInsertRet.second)
                    {
                        bAnalysis = false;
                        break;
                    }
                    
                    m_pCurSection = pSectionNode;
                    break;
                }
                case em_KeyValueLine:
                {
                    pNode ->nIndex = (int)m_pCurSection ->mapSectionAllLineNode.size();
                    std::pair<mapLineNode::iterator, bool> mapInsertRet;
                    mapInsertRet = m_pCurSection ->mapSectionAllLineNode.insert(std::pair<const std::string, _tagIniLineNode *>(pNode ->strKey, pNode));
                    if (!mapInsertRet.second)
                    {
                        bAnalysis = false;
                        break;
                    }
                    break;
                }
                default:
                {
                    bAnalysis = false;
                    break;
                }
            }
            
            if (!bAnalysis)
            {
                if (pNode)
                {
                    delete pNode;
                    pNode = 0;
                }            
                break;
            }
            
            bRet = true;
            break;
        }
        return bRet;
    }
    
    _tagIniLineNode *CIniFile::AnalysisLineToNode(char *pBuf)
    {
        _tagIniLineNode *pRet = 0;
        for (;;)
        {
            int nStrLen = strlen(pBuf);
            if (0 >= nStrLen)
            {
                break;
            }
            if ('\n' != pBuf[nStrLen - 1])
            {
                break;
            }
            
            int nIndex = 0;
            char *pStart = &pBuf[nIndex];
            
            while (' ' == pStart[0])
            {
                nIndex ++;
                pStart = &pBuf[nIndex];
            }
            
            _tagIniLineNode *pNode = new _tagIniLineNode;
            if (!pNode)
            {
                break;
            }
            pNode ->InitIniLineNode();
            
            if ('\r' == pStart[0] || '\n' == pStart[0])
            {
                pNode ->lineType = em_SpaceLine;
            }
            else if (';' == pStart[0])
            {
                pNode ->lineType = em_NoteLine;
                
                char *pTemp = &pStart[1];
                while (' ' == *pTemp)
                {
                    pTemp ++;
                }
                
                int nLenTemp = strlen(pTemp);
                if ('\n' == pTemp[nLenTemp - 1])
                {
                    pTemp[nLenTemp - 1] = 0;
                }
                if ('\r' == pTemp[nLenTemp - 2])
                {
                    pTemp[nLenTemp - 2] = 0;
                }
                pNode ->strNote = pTemp;
            }
            else if ('[' == pStart[0])
            {
                pNode ->lineType = em_SectionLine;
                
                char *pTempStart = &pStart[1];
                while (' ' == *pTempStart)
                {
                    pTempStart ++;
                }
                char *pTempEnd = pTempStart;
                if (']' == *pTempEnd)
                {
                    delete pNode;
                    pNode = 0;
                    break;
                }
                while (']' != *pTempEnd && '\n' != *pTempEnd && 0 != *pTempEnd)
                {
                    pTempEnd ++;
                }
                if (']' != *pTempEnd)
                {
                    delete pNode;
                    pNode = 0;
                    break;
                }
                
                char *pSetionValueEnd = pTempEnd;
                pSetionValueEnd --;
                while (' ' == *pSetionValueEnd)
                {
                    pSetionValueEnd --;
                }
                pSetionValueEnd[1] = 0;
                
                pNode ->strValue = pTempStart;
                
                pTempEnd ++;
                while (';' != *pTempEnd && '\n' != *pTempEnd && 0 != *pTempEnd)
                {
                    pTempEnd ++;
                }
                
                if (';' == *pTempEnd)
                {
                    pTempEnd ++;
                    int nLenTemp = strlen(pTempEnd);
                    if ('\n' == pTempEnd[nLenTemp - 1])
                    {
                        pTempEnd[nLenTemp - 1] = 0;
                    }
                    if ('\r' == pTempEnd[nLenTemp - 2])
                    {
                        pTempEnd[nLenTemp - 2] = 0;
                    }
                    pNode ->strNote = pTempEnd;
                }
                
            }
            else
            {
                pNode ->lineType = em_KeyValueLine;
                
                char *pKeyStart = pStart;
                if ('=' == *pKeyStart)
                {
                    delete pNode;
                    pNode = 0;
                    break;
                }
                
                char *pKeyEnd = &pKeyStart[1];
                while ('=' != *pKeyEnd && '\n' != *pKeyEnd && 0 != *pKeyEnd)
                {
                    pKeyEnd ++;
                }
                
                if ('=' != *pKeyEnd)
                {
                    delete pNode;
                    pNode = 0;
                    break;
                }
                char *pValueStart = &pKeyEnd[1];
                
                pKeyEnd --;
                while (' ' == *pKeyEnd)
                {
                    pKeyEnd --;
                }
                pKeyEnd[1] = 0;
                pNode ->strKey = pKeyStart;
                
                while (' ' == *pValueStart)
                {
                    pValueStart ++;
                }
                
                if (';' == *pValueStart)
                {
                    pValueStart ++;
                    int nLenTemp = strlen(pValueStart);
                    if ('\n' == pValueStart[nLenTemp - 1])
                    {
                        pValueStart[nLenTemp - 1] = 0;
                    }
                    if ('\r' == pValueStart[nLenTemp - 2])
                    {
                        pValueStart[nLenTemp - 2] = 0;
                    }
                    pNode ->strNote = pValueStart;
                }
                else
                {
                    if (!(('\r' == *pValueStart && '\n' == pValueStart[1]) || '\n' == *pValueStart))
                    {
                        char *pNoteStart = 0;
                        char *pValueEnd = 0;
                        int nLenTemp = strlen(pValueStart);
                        for (int i = 0; i < nLenTemp; i ++)
                        {
                            pValueEnd = &pValueStart[i];
                            if (';' == pValueStart[i])
                            {
                                pNoteStart = &pValueStart[i + 1];
                                break;
                            }
                        }
                        
                        pValueEnd --;
                        while (' ' == *pValueEnd || '\r' == *pValueEnd || '\n' == *pValueEnd)
                        {
                            pValueEnd --;
                        }
                        pValueEnd[1] = 0;
                        pNode ->strValue = pValueStart;

                        if (pNoteStart)
                        {
                            int nLenTemp = strlen(pNoteStart);
                            if ('\n' == pNoteStart[nLenTemp - 1])
                            {
                                pNoteStart[nLenTemp - 1] = 0;
                            }
                            if ('\r' == pNoteStart[nLenTemp - 2])
                            {
                                pNoteStart[nLenTemp - 2] = 0;
                            }   
                            pNode ->strNote = pNoteStart;
                        }
                    }
                }  
            }
            
            pRet = pNode;
            break;
        }
        return pRet;
    }
 
}
