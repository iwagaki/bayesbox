#ifndef FILE_OBJECT_H_
#define FILE_OBJECT_H_

#include <cstdio>
#include <vector>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include "common/verify.h"


class IFileObject
{
public:
    virtual ~IFileObject() {}
    virtual void writeObject(FILE* fp) = 0;
    virtual void readObject(FILE* fp) = 0;
};

class FileObject
{
public:
    template<typename T> static void writeObject(FILE* fp, std::vector<T>& v)
    {
        fprintf(fp, "%d\n", v.size());

        for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); ++i)
        {
            IFileObject& file_object = *i;
            file_object.writeObject(fp);
        }
    }

    template<typename T> static void readObject(FILE* fp, std::vector<T>& v)
    {
        T object;
        int size = 0;
        IFileObject& file_object = object;

        VERIFY(fscanf(fp, "%d\n", &size) == 1);

        for(unsigned i = 0; i < size; ++i)
        {
            file_object.readObject(fp);
            v.push_back(object);
        }
    }

    inline static void writeObject(FILE* fp, double v)
    {
        fprintf(fp, "%.15le\n", v);
    }

    inline static double readObject(FILE* fp)
    {
        double element = 0.0;
        VERIFY(fscanf(fp, "%le\n", &element) == 1);

        return element;
    }

    static void writeObject(FILE* fp, boost::numeric::ublas::vector<double>& v)
    {
        fprintf(fp, "%d\n", v.size());

        for (unsigned i = 0; i < v.size(); ++i)
            writeObject(fp, v(i));
    }

    static void readObject(FILE* fp, boost::numeric::ublas::vector<double>& v)
    {
        int size = 0;

        VERIFY(fscanf(fp, "%d\n", &size) == 1);
        v.resize(size);

        for (unsigned i = 0; i < size; ++i)
            v(i) = readObject(fp);
    }

    static void writeObject(FILE* fp, boost::numeric::ublas::matrix<double>& m)
    {
        fprintf(fp, "%d %d\n", m.size1(), m.size2());

        for (unsigned i = 0; i < m.size1(); ++i)
            for (unsigned j = 0; j < m.size2(); ++j)
                writeObject(fp, m(i, j));
    }

    static void readObject(FILE* fp, boost::numeric::ublas::matrix<double>& m)
    {
        int size1 = 0, size2 = 0;

        VERIFY(fscanf(fp, "%d %d\n", &size1, &size2) == 2);
        m.resize(size1, size2);

        for (unsigned i = 0; i < size1; ++i)
            for (unsigned j = 0; j < size2; ++j)
                m(i, j) = readObject(fp);
    }
};

#endif // FILE_OBJECT_H_
