#ifndef LOGGER_H_
#define LOGGER_H_

#include <cstdio>
#include "verify.h"

class Logger
{
public:
    Logger(const char* filename, const char* attr = "w") : m_fp(0)
    {
        VERIFY(filename != 0);

        m_fp = fopen(filename, attr);
        VERIFY(m_fp != 0);
    }

    ~Logger()
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

#endif // LOGGER_H_
