#ifndef __BINARY_BUF_HPP
#define __BINARY_BUF_HPP

namespace zyb
{
    enum ENUM_BINARY_BUF_TYPE
    {
        enumBinaryBuf_unknow = -1,
        enumBinaryBuf_32     = 32,
        enumBinaryBuf_64     = 64,
        enumBinaryBuf_128    = 128,
        enumBinaryBuf_256    = 256,
        enumBinaryBuf_512    = 512,
        enumBinaryBuf_1k     = 1024,
        enumBinaryBuf_2k     = 2 * 1024,
        enumBinaryBuf_4k     = 4 * 1024,
        enumBinaryBuf_8k     = 8 * 1024,
        enumBinaryBuf_16k    = 16 * 1024,
        enumBinaryBuf_32k    = 32 * 1024,
        enumBinaryBuf_64k    = 64 * 1024
    };
    
    class CBinaryBuf
    {
    public:
        CBinaryBuf();
    
        bool CopyBuf(const char *pData, unsigned int nDataSize);
        bool AppendBuf(const char *pData, unsigned int nDataSize);
        
        // 重新布置类中的数据占用存储的内存
        bool Rearrange();
        
        //重新布置 nSeek > 0， 移除前面的数据， nSeek < 0 ， 移除后面的数据
        virtual bool RearrangeSeek(int nSeek) = 0;
        
        // nSeek > 0， 移除前面的数据， nSeek < 0 , 移除后面的数据
        bool Seek(int nSeek);
        
        const char *GetBinaryBuf(unsigned int &nValidBufSize);
        
        void Clear();
        
        ENUM_BINARY_BUF_TYPE GetBinaryBufType() const;
        
        int GetSurplusSize() const;
        
        bool IsEmpty() const;
    protected:
        bool RearrangeSeek(char *pBuf, unsigned int nBufSize);
        
    protected:
        char *m_pData;
        ENUM_BINARY_BUF_TYPE m_emBinaryBufType;
        unsigned int m_unValidStartIndex;
        unsigned int m_unValidEndIndex;
    };
    
    class CBinaryBuf32 : public CBinaryBuf
    {
    public:
        CBinaryBuf32();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_32];
    };
    
    class CBinaryBuf64 : public CBinaryBuf
    {
    public:
        CBinaryBuf64();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_64];
    };
    
    class CBinaryBuf128 : public CBinaryBuf
    {
        public:
        CBinaryBuf128();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_128];
    };
    
    class CBinaryBuf256 : public CBinaryBuf
    {
        public:
        CBinaryBuf256();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_256];
    };
    
    class CBinaryBuf512 : public CBinaryBuf
    {
        public:
        CBinaryBuf512();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_512];
    };
    
    class CBinaryBuf1k : public CBinaryBuf
    {
        public:
        CBinaryBuf1k();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_1k];
    };
    
    class CBinaryBuf2k : public CBinaryBuf
    {
        public:
        CBinaryBuf2k();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_2k];
    };
    
    class CBinaryBuf4k : public CBinaryBuf
    {
        public:
        CBinaryBuf4k();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_4k];
    };
    
    class CBinaryBuf8k : public CBinaryBuf
    {
        public:
        CBinaryBuf8k();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_8k];
    };
    
    class CBinaryBuf16k : public CBinaryBuf
    {
        public:
        CBinaryBuf16k();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_16k];
    };
    
    class CBinaryBuf32k : public CBinaryBuf
    {
        public:
        CBinaryBuf32k();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_32k];
    };
    
    class CBinaryBuf64k : public CBinaryBuf
    {
        public:
        CBinaryBuf64k();
        
        virtual bool RearrangeSeek(int nSeek);
    private:
        char m_szData[enumBinaryBuf_64k];
    };
}

#endif // __BINARY_BUF_HPP