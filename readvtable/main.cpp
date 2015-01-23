/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  i
 *
 *        Version:  1.0
 *        Created:  05/24/2013 10:13:29 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Costaxu (), costaxu@tencent.com
 *        Company:  Tencent, China
 *
 * =====================================================================================
 */


#include <elf.h>
#include <assert.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>
using namespace std;

class ClsSectionShstrtab;
class ClsSectionStrtab;
class ClsSectionSymtab;
class ClsSectionHeaders;
class ClsSectionRodata;

void usage()
{
    printf("readvtable excutable\n");
}

typedef struct{
    ClsSectionHeaders       * poHeaders;
    ClsSectionStrtab        * poStrtab;
    ClsSectionShstrtab      * poShstrtab;
    ClsSectionSymtab        * poSymtab;
    ClsSectionRodata        * poRodata;


}GlobalObject;

GlobalObject g_section;

const char* g_pcBuffer = 0;

template <typename EntryType>
class ClsSection
{
    public:
    ClsSection(){}
    ClsSection(Elf32_Shdr* hdr)
        :m_hdr(hdr)
    {
        m_pcBuffer = g_pcBuffer + hdr->sh_offset; 
        m_iLen     = hdr->sh_size;
        //m_iEntryCount = hdr->sh_entsize;
        m_iEntryCount = m_iLen/sizeof(EntryType);
    }

    Elf32_Shdr* m_hdr;     
    const char* m_pcBuffer;
    size_t      m_iLen;
    uint32_t    m_iEntryCount;
    
    virtual int ReadFromBuffer() 
    {
        return 0;
    }

    virtual EntryType* Get (uint32_t i)
    {
        EntryType* array = (EntryType*) m_pcBuffer; 
        if(i<m_iEntryCount)
        {
            return array+i;
        }
        return 0;
    }

};

typedef struct{} None;

class ClsSectionShstrtab:public ClsSection<None>
{
    public:
        ClsSectionShstrtab()
        {}

        ClsSectionShstrtab(Elf32_Shdr* hdr)
            :ClsSection<None>(hdr)
        {
        }

        
        vector<string> m_vecString;

        virtual int ReadFromBuffer()
        {
            int iStartFlag = 0;
            const char* pcBuffer = m_pcBuffer;
            const char* pc = pcBuffer;                
            while( (pc - pcBuffer) < (int)m_iLen)
            {
                
                if(iStartFlag)
                {
                    m_vecString.push_back(string(pc));
                    iStartFlag = 0;
                }
                if(*pc == 0)
                {
                    iStartFlag = 1;
                }

                pc++;
            }
            /*
            if(iStartFlag == 1)
            {

                printf("Err: %s:%d:  position: %d\n",__func__, __LINE__, pc-pcBuffer);
                return 0; 
            }

            */
            //printf("Err: %s:%d:  position: %d\n",__func__, __LINE__, pc-pcBuffer);
            return 0;
        }

        virtual void Print()
        {
            uint32_t i=0;
            for(;i<m_vecString.size();i++)
            {
                printf("%5d: %s\n",i, m_vecString[i].c_str());
            }
        }

        virtual string  GetString(Elf32_Word ndx) const
        {
            if(ndx>=0 && ndx<= m_iLen-1)
            {
                return string(m_pcBuffer+ndx); 
            } 
            return "";
        }
};

class ClsSectionStrtab:public ClsSectionShstrtab
{
    public:
    ClsSectionStrtab() {}

    ClsSectionStrtab(Elf32_Shdr* hdr) 
        :ClsSectionShstrtab(hdr)
    {

    }
};

class ClsSectionHeaders
{
    public:
    ClsSectionHeaders(Elf32_Off offset, Elf32_Half shEntryCount )
        :m_iOffset(offset),  m_shEntryCount(shEntryCount)
    {
        m_pcBuffer = g_pcBuffer + offset;     

    }    

    Elf32_Off  m_iOffset;
    Elf32_Half m_shEntryCount;
    const char* m_pcBuffer;
    Elf32_Shdr* m_arrSectionHeader;

    virtual int ReadFromBuffer()
    {
        m_arrSectionHeader = (Elf32_Shdr*) m_pcBuffer;        
        return 0;
    }

    Elf32_Shdr* Get(unsigned i)
    {
        if(i<=m_shEntryCount)
        {
            return m_arrSectionHeader+i;
        }
        else
        {
            printf("Err: %s:%d You are fetching an unexist section\n", __func__, __LINE__);
            return 0;
        }
    }
    
    Elf32_Shdr * GetByName(const string & strSectionName  )
    {
        const ClsSectionShstrtab& objShstr = * g_section.poShstrtab;
        unsigned i =1; 
        for(;i<m_shEntryCount;i++)
        {
            Elf32_Shdr* pSec = Get(i); 
            if(pSec==0)return 0;
            string strName = objShstr.GetString(pSec->sh_name);
            if(strName == strSectionName)
                return pSec;
        }
        return 0;
    }
};

class ClsSectionRodata:public ClsSection<None>
{
    public:
    ClsSectionRodata(Elf32_Shdr* hdr)
        :ClsSection<None>(hdr)
    {
    }

};

class SymbolVtable
{
    public:
    typedef Elf32_Sym EntryType;
    string m_sName;
    EntryType * m_pEntry;

    const char* m_pcBuffer;
    size_t      m_iLen;

    SymbolVtable(string& sName,EntryType* pEntry)
        :m_sName(sName),m_pEntry(pEntry)
    {
        const ClsSectionRodata & objRodata = *g_section.poRodata;
        m_pcBuffer = pEntry->st_value - objRodata.m_hdr->sh_addr + objRodata.m_pcBuffer;  
        m_iLen       = pEntry->st_size;
    }

    };

class ClsSectionSymtab:public ClsSection<Elf32_Sym>
{

    public:

    typedef Elf32_Sym EntryType;
    
    ClsSectionSymtab(Elf32_Shdr * hdr)
        :ClsSection<EntryType>(hdr)
    {

    }

    EntryType*  m_array;
    map<string , EntryType*> m_map;
    map<uint32_t,string> m_map_addr;
    vector<SymbolVtable> m_vecSymbolVtable;
    typedef map<string,EntryType*> SymMap;
    typedef SymMap::iterator SymMapIter;
    typedef map<uint32_t,string> SymAddrMap;
    typedef SymAddrMap::iterator SymAddrMapIter;

    virtual int ReadFromBuffer()
    {
        m_array = (Elf32_Sym*)m_pcBuffer;
        int iRet = ReadToMap();
        if(iRet)return iRet;
        return 0;
    }


    virtual int ReadToMap()
    {
        const ClsSectionShstrtab& objStrtab = *(g_section.poStrtab);
        EntryType* pEntry = 0;
        unsigned i =0;
        for(;i<m_iEntryCount;i++)
        {
            pEntry = Get(i);
            string sName = objStrtab.GetString(pEntry->st_name);
            if(sName != "")
            {
                m_map[sName] = pEntry;
                m_map_addr[pEntry->st_value] =sName;
            } 
        }
        return 0;
    }

    virtual void PrintMap()
    {
         SymMapIter iter = m_map.begin();
         for(;iter!=m_map.end();iter++)
         {
            printf("%s\n", iter->first.c_str());
            
         }
    }

    virtual void Print()
    {
        const ClsSectionShstrtab& objStrtab = *(g_section.poStrtab);
        EntryType* pEntry = 0;
        unsigned i =0;
        printf("      name\t     value\t      size\t      info\t      other\t      ndx\n");
        for(;i<m_iEntryCount;i++)
        {
            pEntry = Get(i);
            string sName = objStrtab.GetString(pEntry->st_name);
            if(strncmp(sName.c_str(), "_ZTV", 4)==0 &&
                    sName.find("@")==string::npos)
            {
                printf("%10s\t%10x\t%10u\t%10hhu%10hhu%10hu\n",
                    //pEntry->st_name,
                    sName.c_str(),
                    pEntry->st_value,
                    pEntry->st_size,
                    pEntry->st_info,
                    pEntry->st_other,
                    pEntry->st_shndx
                    );
            }
        }
    }

    virtual void FindVtable()
    {
        const ClsSectionShstrtab& objStrtab = *(g_section.poStrtab);
        EntryType* pEntry = 0;
        unsigned i =0;
        for(;i<m_iEntryCount;i++)
        {
            pEntry = Get(i);
            string sName = objStrtab.GetString(pEntry->st_name);
            if(strncmp(sName.c_str(), "_ZTV", 4)==0 &&
                    sName.find("@")==string::npos)
            {
                SymbolVtable sv(sName,pEntry);
                PrintSymVtable(&sv);
                m_vecSymbolVtable.push_back(sv);
            }
        }

         
    }

    void PrintSymVtable(SymbolVtable * psym)
    {
        const char* pc = psym->m_pcBuffer;
        printf("%s\n", psym->m_sName.c_str());
        while((pc - psym->m_pcBuffer) < (int)psym->m_iLen)
        {
            uint32_t tmp =  * (uint32_t*)pc;
            string sName = GetByAddr(tmp);
            printf("%08x\t%s\n",tmp,sName.c_str());
            pc+=4;
        }
    }

    string GetByAddr(uint32_t addr)
    {
        SymAddrMapIter it = m_map_addr.find(addr);
        if(it==m_map_addr.end())
            return "";
        else
            return it->second;
    }


};


int main(int argc,char** argv)
{
    if(argc == 1)
    {
        usage();
        return 0;
    }

    const char* pcFileName = argv[1];

    FILE *pFile = fopen(pcFileName, "r");
    if(pFile == 0)
    {
        printf("%s not exist\n", pcFileName);
        return -1;
    }

    fseek(pFile, 0, SEEK_END);
    long iFileLen = ftell(pFile);

    printf("File size %ld bytes\n", iFileLen);
    
    char * pcContent = (char*)malloc(iFileLen); 
    g_pcBuffer = pcContent; 

    fseek(pFile , 0 ,SEEK_SET);

    fread( pcContent , 1 , iFileLen , pFile );
    fclose(pFile);

    Elf32_Ehdr * pElfHeader = (Elf32_Ehdr*) pcContent;


    uint16_t shShstrndx= pElfHeader->e_shstrndx ;
    printf("section table Header = %u\n", pElfHeader->e_shoff);

    
    ClsSectionHeaders oHeaders(pElfHeader->e_shoff, pElfHeader->e_shnum );
    oHeaders.ReadFromBuffer();
    g_section.poHeaders = &oHeaders;

    Elf32_Shdr * pSecShstrtab = oHeaders.Get(shShstrndx);
    ClsSectionShstrtab  oSectionShstr (pSecShstrtab);
    oSectionShstr.ReadFromBuffer();
    g_section.poShstrtab = &oSectionShstr;
    //oSectionShstr.Print();


    
    Elf32_Shdr * pSecStrtab = oHeaders.GetByName(".strtab" );
    assert(pSecStrtab);
    ClsSectionStrtab oSectionStrtab(pSecStrtab);
    assert(oSectionStrtab.ReadFromBuffer()==0);
    g_section.poStrtab = &oSectionStrtab;
    //oSectionStrtab.Print();

   
    
    Elf32_Shdr * pSecRodata= oHeaders.GetByName(".rodata");
    ClsSectionRodata objRodata(pSecRodata);
    assert(objRodata.ReadFromBuffer()==0);
    g_section.poRodata= &objRodata;

    Elf32_Shdr * pSecSymtab = oHeaders.GetByName(".symtab");
    ClsSectionSymtab objSectionSymtab(pSecSymtab);
    assert(objSectionSymtab.ReadFromBuffer()==0);
    g_section.poSymtab = &objSectionSymtab;
    objSectionSymtab.Print();
    objSectionSymtab.FindVtable();
 
    return 0;  
}
