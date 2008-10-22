#ifndef FILE_DESCRIPTOR_H_
#define FILE_DESCRIPTOR_H_

#include <cstdio>
#include "verify.h"


class FileDescriptor
{
public:
    FileDescriptor(const char* filename, const char* attr = "wb") : m_fp(0)
    {
        VERIFY(filename != 0);

        m_fp = fopen(filename, attr);
        VERIFY(m_fp != 0);
    }

    virtual ~FileDescriptor()
    {
        if (m_fp)
        {
            int rc = fclose(m_fp);
            assert(rc == 0);
            m_fp = 0;
        }
    }

    operator FILE*() const
    {
        return fp();
    }

    FILE* fp() const
    {
        VERIFY(m_fp != 0);
        return m_fp;
    }

private:
    FILE* m_fp;
};


class FileReader : public FileDescriptor
{
public:
    FileReader(const char* filename) : FileDescriptor(filename, "rb") {}
};


class FileWriter : public FileDescriptor
{
public:
    FileWriter(const char* filename) : FileDescriptor(filename, "wb") {}
};


#endif // FILE_DESCRIPTOR_H_
