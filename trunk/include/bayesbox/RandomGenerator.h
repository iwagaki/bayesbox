#ifndef RANDOM_GENERATOR_H_
#define RANDOM_GENERATOR_H_

#include <sys/time.h>

// #include <cstdlib>
// #include <ctime>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/random.hpp>

#include "common/verify.h"

using namespace boost::numeric;

class RandomGenerator
{
public:
    RandomGenerator()
    {
        struct timeval tv;
        struct timezone tz;
        int rc = gettimeofday(&tv, &tz);
        VERIFY(rc == 0)

        unsigned long long now = tv.tv_sec * 1000 * 1000 + tv.tv_usec;

        m_boost_rng = new boost::mt19937(static_cast<unsigned>(now));
    }

    ~RandomGenerator()
    {
        delete m_boost_rng;
    }

    double gaussian(double sigma)
    {
        boost::normal_distribution<> normal(0.0, sigma);
        boost::variate_generator< boost::mt19937&, boost::normal_distribution<double> > normal_sampler(*m_boost_rng, normal);

        return normal_sampler();
    }

    double uniform()
    {
        boost::uniform_real<> uniform(0.0, 1.0);
        boost::variate_generator< boost::mt19937&, boost::uniform_real<> > uniform_sampler(*m_boost_rng, uniform);

        return uniform_sampler();
    }

//     void gaussian(gsl_vector* pVector, double sigma, double coef, double cons)
//     {
//         for (size_t i = 0; i < pVector->size; ++i)
//         {
//             gsl_vector_set(pVector, i, gsl_vector_get(pVector, i) + gsl_ran_gaussian(m_rng, sigma) * coef + cons);
//         }
//     }

    ublas::vector<double> gaussian(double sigma, unsigned size)
    {
        ublas::vector<double> r(size);

        for (unsigned i = 0; i < size; ++i)
            r(i) = gaussian(sigma);
        
        return r;
    }

    ublas::matrix<double> gaussian(double sigma, unsigned size1, unsigned size2)
    {
        ublas::matrix<double> r(size1, size2);

        for (unsigned i = 0; i < size1; ++i)
            for (unsigned j = 0; j < size2; ++j)
                r(i, j) = gaussian(sigma);
        
        return r;
    }

    ublas::vector<double> gaussian(ublas::vector<double> v, double sigma, double coef, double cons)
    {
        for (unsigned i = 0; i < v.size(); ++i)
        {
            v(i) += gaussian(sigma) * coef + cons;
        }
        
        return v;
    }
    
    ublas::vector<double> uniform(ublas::vector<double> v, double coef, double cons)
    {
        for (unsigned i = 0; i < v.size(); ++i)
        {
            v(i) += uniform() * coef + cons;
        }
        
        return v;
    }
        
    ublas::matrix<double> uniform(ublas::matrix<double> m, double coef, double cons)
    {
        for (unsigned i = 0; i < m.size1(); ++i)
        {
            for (unsigned j = 0; j < m.size2(); ++j)
            {
                m(i, j) += uniform() * coef + cons;
            }
        }

        return m;
    }

    ublas::matrix<double> uniform2(ublas::matrix<double> m, double coef, double cons, int n)
    {
        unsigned i = n % m.size1();
        unsigned j = (n / m.size1()) % m.size2();

        m(i, j) += uniform() * coef + cons;

        return m;
    }

    ublas::vector<double> uniform2(ublas::vector<double> v, double coef, double cons, int n)
    {
        unsigned i = n % v.size();

        v(i) += uniform() * coef + cons;

        return v;
    }

private:
    boost::mt19937* m_boost_rng;
};


#endif // RANDOM_GENERATOR_H_
