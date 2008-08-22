#ifndef GAUSSIAN_PARAMETER_H_
#define GAUSSIAN_PARAMETER_H_

#include <cstdio>
#include <cmath>
#include <ctime>
#include <cassert>
#include <stdint.h>
#include <vector>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>

#include "RandomGenerator.h"
#include "Bayes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // memset()
#include <stdint.h>     // uint_t, int_t
#include <assert.h>     // assert()

#include "common/verify.h"

using namespace boost::numeric;


class GaussianParameter : public IMCMCParameter
{
public:
    GaussianParameter(RandomGenerator& rng) : m_pRng(&rng), m_step(0.0), m_temperature(1.0) {}

    ~GaussianParameter() {}

    ublas::vector<double> value(const ublas::vector<double>& x) const
    {
        return x;
    }

    void setParameter(ublas::matrix<double>& w1)
    {
        m_w1 = w1;
    }
    
    void setStep(double step) { m_step = step; }

    void setTemperature(double temperature) { m_temperature = temperature; }

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
        double norm2 = 0.0;

        ublas::matrix<double> w1 = m_w1;

        for (unsigned i = 0; i < w1.size1(); ++i)
        {
            ublas::matrix_row< ublas::matrix<double> > mr(w1, i);

            norm2 += inner_prod(mr, mr);
        }
                
        return exp(-norm2 / (2 * 100));
    }

    double energy(std::vector<Data>& dataSet, double& h)
    {
        double sum1 = 0.0;
        
        for (unsigned i = 0; i < dataSet.size(); ++i)
        {
            double sum2 = 0.0;

            for (unsigned j = 0; j < m_w1.size1(); ++j)
            {
                ublas::vector<double> mu(1);
                mu(0) = m_w1(j, 1);

                ublas::vector<double> r = dataSet[i].y - mu;
                sum2 += m_w1(j, 0) * exp(- inner_prod(r, r) / 2.0);
            }

            sum1 += log(sum2 / sqrt(2 * 3.14));
        }

        h = sum1;

        return -log(prior()) + m_temperature * h;
    }

    void print(FILE* fp)
    {
        for (unsigned i = 0; i < m_w1.size1(); ++i)
            for (unsigned j = 0; j < m_w1.size2(); ++j)
                fprintf(fp, "%f, ", m_w1(i, j));

        fprintf(fp, "\n");
    }

    void next(unsigned i)
    {
        m_tmp_w1 = m_w1;

        double tmp_step = m_step;

        m_w1 = m_pRng->uniform2(m_w1, tmp_step * 2.0, -tmp_step, i);
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
        GaussianParameter* p = new GaussianParameter(*m_pRng);
        *p = *this;

        paramSet.push_back(p);
    }

    void swap(IMCMCParameter* pParam)
    {
        GaussianParameter* pDst = dynamic_cast<GaussianParameter*>(pParam);

        ublas::matrix<double> tmp1 = m_w1;
        
        m_w1 = pDst->m_w1;
        pDst->m_w1 = tmp1;
    }
    
// private:
public:
    RandomGenerator* m_pRng;

    ublas::matrix<double> m_w1;

    double m_step;
    double m_temperature;

    ublas::matrix<double> m_tmp_w1;
};


#endif // GAUSSIAN_PARAMETER_H_
