#ifndef OPTION_PARSER_H_
#define OPTION_PARSER_H_

#include <cstdlib>
#include <vector>
#include "common/verify.h"

class OptionHolder
{
public:
    OptionHolder(const char* pIdentifier, const char* pDescription) :
        m_pIdentifier(pIdentifier), m_pDescription(pDescription) {}

    virtual void help() = 0;
    virtual void setValue(const char* pString) = 0;

    const char* m_pIdentifier;
    const char* m_pDescription;
};


class OptionHolderInt : public OptionHolder
{
public:
    OptionHolderInt(int value, const char* pIdentifier, const char* pDescription) :
        OptionHolder(pIdentifier, pDescription), m_value(value) {}

    void help()
    {
        printf("    --%-30s  %s (%d)\n", m_pIdentifier, m_pDescription, m_value);
    }

    void setValue(const char* pString)
    {
        VERIFY(pString != 0);
        m_value = atoi(pString);
    }
    
    int m_value;
};


class OptionHolderDouble : public OptionHolder
{
public:
    OptionHolderDouble(double value, const char* pIdentifier, const char* pDescription) :
        OptionHolder(pIdentifier, pDescription), m_value(value) {}

    void help()
    {
        printf("    --%-30s  %s (%f)\n", m_pIdentifier, m_pDescription, m_value);
    }

    void setValue(const char* pString)
    {
        VERIFY(pString != 0);
        m_value = atof(pString);
    }
    
    double m_value;
};


class OptionHolderString : public OptionHolder
{
public:
    OptionHolderString(const char* value, const char* pIdentifier, const char* pDescription) :
        OptionHolder(pIdentifier, pDescription), m_value(value) {}
    
    void help()
    {
        const char* value = m_value ? m_value : "NULL";
        printf("    --%-30s  %s (%s)\n", m_pIdentifier, m_pDescription, value);
    }

    void setValue(const char* pString)
    {
        VERIFY(pString != 0);
        m_value = pString;
    }
    
    const char* m_value;
};


class OptionHolderBool : public OptionHolder
{
public:
    OptionHolderBool(bool value, const char* pIdentifier, const char* pDescription) :
        OptionHolder(pIdentifier, pDescription), m_value(value), m_default(value) {}
    
    void help()
    {
        printf("    --%-30s  %s (%d)\n", m_pIdentifier, m_pDescription, (int)m_value);
    }

    void setValue(const char* pString)
    {
        VERIFY(pString == 0);
        m_value = !m_default;
    }
    
    bool m_value;
    bool m_default;
};


class OptionParser
{
public:
    static int& createOption(int value, const char* pIdentifier, const char* pDescription)
    {
        OptionHolderInt* p = new OptionHolderInt(value, pIdentifier, pDescription);
        m_optionList.push_back(p);

        return p->m_value;
    }

    static double& createOption(double value, const char* pIdentifier, const char* pDescription)
    {
        OptionHolderDouble* p = new OptionHolderDouble(value, pIdentifier, pDescription);
        m_optionList.push_back(p);

        return p->m_value;
    }

    static const char*& createOption(const char* value, const char* pIdentifier, const char* pDescription)
    {
        OptionHolderString* p = new OptionHolderString(value, pIdentifier, pDescription);
        m_optionList.push_back(p);

        return p->m_value;
    }
    
    static bool& createOption(bool value, const char* pIdentifier, const char* pDescription)
    {
        OptionHolderBool* p = new OptionHolderBool(value, pIdentifier, pDescription);
        m_optionList.push_back(p);

        return p->m_value;
    }

    static void helpAll()
    {
        printf("Options:\n");

        for (std::vector<OptionHolder*>::iterator i = m_optionList.begin(); i != m_optionList.end(); ++i)
            (*i)->help();
    }

    static void parse(int argc, char** argv)
    {
        int index = 1;

        while (index < argc)
        {
            bool isFound = false;

            char* pParameter;
            const char* pIdentifier = parse1Param(index, pParameter, argc, argv);

            for (std::vector<OptionHolder*>::iterator i = m_optionList.begin(); i != m_optionList.end(); ++i)
            {
                if (strcmp(pIdentifier, (*i)->m_pIdentifier) == 0)
                {
                    (*i)->setValue(pParameter);
                    isFound = true;
                    break;
                }
            }

            if (!isFound)
            {
                helpAll();
                exit(-1);
            }
        }
    }

    static const char* parse1Param(int& index, char*& pParameter, int argc, char** argv)
    {
        if (index >= argc)
            VERIFY(0);

        char* p = argv[index];

        if (strncmp(p, "--", 2) != 0)
            VERIFY("invalid parameter" && 0);
        
        p += 2;
        ++index;
        pParameter = 0;

        if (index < argc)
        {
            char* p2 = argv[index];
        
            if (strncmp(p2, "--", 2) != 0)
            {
                pParameter = p2;
                ++index;
            }
        }
        
        return p;
    }

private:
    static std::vector<OptionHolder*> m_optionList;
};


#endif // OPTION_PARSER_H_
