#ifndef MIXTURE_PARAMETER_H_
#define MIXTURE_PARAMETER_H_

#include <cstdio>
#include <cmath>
#include <ctime>
#include <cassert>
#include <stdint.h>
#include <vector>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include "bayesbox/RandomGenerator.h"
#include "bayesbox/Bayes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // memset()
#include <stdint.h>     // uint_t, int_t
#include <assert.h>     // assert()

#include "common/verify.h"

using namespace boost::numeric;


class MixtureParameter : public IMCMCParameter
{
public:
    MixtureParameter() : m_pRng(0), m_temperature(1.0) {}

    ~MixtureParameter() {}

    ublas::vector<double> value(const ublas::vector<double>& x) const
    {
        return x;
    }

    void setParameter(ublas::matrix<double>& w1)
    {
        m_w1 = w1;
    }
    
    void setStep(ublas::matrix<double>& step)
    {
        m_step = step;
    }

    void setTemperature(double temperature) { m_temperature = temperature; }

    void setRng(RandomGenerator& rng) { m_pRng = &rng; }

    double getTemperature() { return m_temperature; }

    ublas::vector<double> createData()
    {
        VERIFY(m_w1.size1() == 1);

        ublas::vector<double> mu(1);
        mu(0) = m_w1(0, 1);
        double sigma = m_w1(0, 2);

        return m_pRng->gaussian(mu, sigma, (double)1.0, (double)0.0);
    }
    
    double prior()
    {
        double rangeout = 0.0;

        if (m_w1(0, 0) < 0.0)
            rangeout = 0.0 - m_w1(0, 0);
                                  
        if (m_w1(0, 0) > 1.0)
            rangeout = m_w1(0, 0) - 1.0;

        return (m_w1(0, 1) * m_w1(0, 1) + m_w1(1, 1) * m_w1(1, 1)) / 50.0 + rangeout * rangeout * 10000.0;
    }

    double energy(std::vector<Data>& dataSet, double& h)
    {
        double sum1 = 0.0;
        
        for (unsigned i = 0; i < dataSet.size(); ++i)
        {
            double sum2 = 0.0;

            for (unsigned j = 0; j < m_w1.size1(); ++j)
            {
//                 printf("0 alpha = %f\n", m_w1(0, 0));
//                 printf("0 mu = %f\n", m_w1(0, 1));
//                 printf("0 sigma = %f\n", m_w1(0, 2));

//                 printf("1 alpha = %f\n", m_w1(1, 0));
//                 printf("1 mu = %f\n", m_w1(1, 1));
//                 printf("1 sigma = %f\n", m_w1(1, 2));

                ublas::vector<double> mu(1);
                mu(0) = m_w1(j, 1);

                ublas::vector<double> r = dataSet[i].y - mu;
                sum2 += m_w1(j, 0) * exp(- inner_prod(r, r) / 2.0);
            }

            sum1 += -log(sum2 / sqrt(2 * 3.14));

//            printf("sum1 = %f\n", sum1);
//          printf("sum2 = %f\n", sum2);
        }

        h = sum1;

//        printf("e = %f\n", prior() + m_temperature * h);
        return prior() + m_temperature * h;
    }

    void print(FILE* fp)
    {
        for (unsigned i = 0; i < m_w1.size1(); ++i)
            for (unsigned j = 0; j < m_w1.size2(); ++j)
                fprintf(fp, "%f  ", m_w1(i, j));

        fprintf(fp, "\n");
    }

    void next(unsigned i)
    {
        m_tmp_w1 = m_w1;

        if ((i % 3) == 0)
        {
            m_w1(0, 0) += (m_pRng->uniform() * 2.0 - 1.0) * m_step(0, 0);
            m_w1(1, 0) = 1.0 - m_w1(0, 0);
        }

        if ((i % 3) == 1)
            m_w1(0, 1) += (m_pRng->uniform() * 2.0 - 1.0) * m_step(0, 1);

        if ((i % 3) == 2)
            m_w1(1, 1) += (m_pRng->uniform() * 2.0 - 1.0) * m_step(1, 1);

//        print(stdout);
    }

    void accept()
    {
        // nothing to do
    }

    void reject()
    {
        m_w1 = m_tmp_w1;
    }

    void push(std::vector<IMCMCParameter*>& paramSet)
    {
        MixtureParameter* p = new MixtureParameter();
        *p = *this;

        paramSet.push_back(p);
    }

    void swap(IMCMCParameter* pParam)
    {
        MixtureParameter* pDst = dynamic_cast<MixtureParameter*>(pParam);

        ublas::matrix<double> tmp1 = m_w1;
        
        m_w1 = pDst->m_w1;
        pDst->m_w1 = tmp1;
    }
    
// private:
public:
    RandomGenerator* m_pRng;

    ublas::matrix<double> m_w1;

    ublas::matrix<double> m_step;
    double m_temperature;

    ublas::matrix<double> m_tmp_w1;
};


#endif // MIXTURE_PARAMETER_H_
