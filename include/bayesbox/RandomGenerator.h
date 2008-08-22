#ifndef RANDOM_GENERATOR_H_
#define RANDOM_GENERATOR_H_

#include <cstdlib>
#include <ctime>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
//#include <boost/random.hpp>

using namespace boost::numeric;

class RandomGenerator
{
public:
    RandomGenerator() : m_rng(0)
    {
        m_rng = gsl_rng_alloc(gsl_rng_default);

        if (!m_rng)
            abort();

        gsl_rng_set(m_rng, time(0));
    }

    ~RandomGenerator()
    {
        gsl_rng_free(m_rng);
    }
    
    double gaussian(double sigma)
    {
        return gsl_ran_gaussian(m_rng, sigma);
    }

    double uniform()
    {
        return gsl_rng_uniform(m_rng);
    }

//     void gaussian(gsl_vector* pVector, double sigma, double coef, double cons)
//     {
//         for (size_t i = 0; i < pVector->size; ++i)
//         {
//             gsl_vector_set(pVector, i, gsl_vector_get(pVector, i) + gsl_ran_gaussian(m_rng, sigma) * coef + cons);
//         }
//     }

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
        
//     void uniform(gsl_vector* pVector, double coef, double cons)
//     {
//         for (size_t i = 0; i < pVector->size; ++i)
//         {
//             gsl_vector_set(pVector, i, gsl_vector_get(pVector, i) + gsl_rng_uniform(m_rng) * coef + cons);
//         }
//     }

//     void gaussian(gsl_matrix* pMatrix, double sigma, double coef, double cons)
//     {
//         for (size_t i = 0; i < pMatrix->size1; ++i)
//         {
//             for (size_t j = 0; j < pMatrix->size2; ++j)
//             {
//                 gsl_matrix_set(pMatrix, i, j, gsl_matrix_get(pMatrix, i, j) + gsl_ran_gaussian(m_rng, sigma) * coef + cons);
//             }
//         }
//     }

//     void uniform(gsl_matrix* pMatrix, double coef, double cons)
//     {
//         for (size_t i = 0; i < pMatrix->size1; ++i)
//         {
//             for (size_t j = 0; j < pMatrix->size2; ++j)
//             {
//                 gsl_matrix_set(pMatrix, i, j, gsl_matrix_get(pMatrix, i, j) + gsl_rng_uniform(m_rng) * coef + cons);
//             }
//         }
//     }

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
    // boost::variate_generator< boost::mt19937, boost::uniform_real<> > m_rngUniform;
    // boost::variate_generator< boost::normal_distribution, boost::uniform_real<> > m_rngUniform;
    


    gsl_rng *m_rng;
};


#endif // RANDOM_GENERATOR_H_
