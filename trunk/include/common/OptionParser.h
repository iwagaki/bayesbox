#ifndef OPTION_PARSER_H_
#define OPTION_PARSER_H_

#include <cstdlib>
#include "common/verify.h"

class OptionParser
{
public:
    OptionParser(int argc, char** argv) : m_argc(argc), m_argv(argv) {}
    
    int getInt(char ch, int defaultValue)
    {
        char* pParameter;
        if (parseParam(ch, pParameter))
        {
            if (!pParameter)
                VERIFY("invalid parameter" && 0);
            
            return atoi(pParameter);
        }
        
        return defaultValue;
    }
 
    double getFloat(char ch, double defaultValue)
    {
        char* pParameter;
        if (parseParam(ch, pParameter))
        {
            if (!pParameter)
                VERIFY("invalid parameter" && 0);
            
            return atof(pParameter);
        }
        
        return defaultValue;
    }

    const char* getString(char ch, const char* defaultValue)
    {
        char* pParameter;
        if (parseParam(ch, pParameter))
        {
            if (!pParameter)
                VERIFY("invalid parameter" && 0);
            
            return pParameter;
        }
        
        return defaultValue;
    }

    bool isFlag(char ch)
    {
        char* pParameter;
        bool isFlag = parseParam(ch, pParameter);

        if (isFlag && pParameter)
            VERIFY("invalid parameter" && 0);

        return isFlag;
    }

    bool parseParam(char ch, char*& pParameter)
    {
        int index = 1;

        while (index < m_argc)
        {
            if (parse1Param(index, pParameter) == ch)
                return true;
        }

        return false;
    }

    char parse1Param(int& index, char*& pParameter)
    {
        if (index >= m_argc)
            VERIFY(0);

        char* p = m_argv[index];
        char ch;

        if (*p++ != '-')
            VERIFY("invalid parameter" && 0);

        ch = *p++;

        if (ch < 'a' && ch > 'Z')
            VERIFY("invalid parameter" && 0);

        if (*p != 0)
            VERIFY("invalid parameter" && 0);

        ++index;
        pParameter = 0;

        if (index < m_argc)
        {
            p = m_argv[index];
        
            if (*p != '-')
            {
                pParameter = p;
                ++index;
            }
        }

        return ch;
    }

private:
    int m_argc;
    char** m_argv;
};

#endif // OPTION_PARSER_H_
