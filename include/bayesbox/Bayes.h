#ifndef BAYES_H_
#define BAYES_H_

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // memset()
#include <stdint.h>     // uint_t, int_t
#include <assert.h>     // assert()

using namespace boost::numeric;

struct Data
{
    ublas::vector<double> x;
    ublas::vector<double> y;
};


class IParameter
{
public:
    virtual ~IParameter() {}
    virtual void print(FILE* fp) = 0;

    virtual ublas::vector<double> value(const ublas::vector<double>& x) const = 0;
};


class IMCMCParameter : public IParameter
{
public:
    virtual ~IMCMCParameter() {}
    virtual double energy(std::vector<Data>& dataSet, double& h) = 0;
    virtual void next(unsigned i) = 0;
    virtual void accept() = 0;
    virtual void reject() = 0;
    virtual void push(std::vector<IMCMCParameter*>& paramSet) = 0;
    virtual double getTemperature() = 0;
    virtual void setTemperature(double temperature) = 0;
    virtual void swap(IMCMCParameter* pParam) = 0;
};


template <typename T> void swapPointer(T* a, T* b) { T* tmp = a; a = b; b = tmp; }


class Model
{
public:
    Model(RandomGenerator& rng) : m_pRng(&rng), m_sigma(0), m_Bt(0.0), m_Gt(0.0) {}
    ~Model() {}

    void MCMC(int num, int skip, int step, std::vector<IMCMCParameter*>& replicaSet)
    {
        unsigned acceptNum = 0;
        unsigned stepCount = 0;
        
        int numOfReplica = replicaSet.size();

        double exchangeAcceptCount[numOfReplica];
        double exchangeTotalCount[numOfReplica];

        double samplingAcceptCount[numOfReplica];
        double samplingTotalCount[numOfReplica];

        double E[numOfReplica];
        double H[numOfReplica];

        for (int l = 0; l < numOfReplica; ++l)
        {
            exchangeAcceptCount[l] = 0;
            exchangeTotalCount[l] = 0;
            samplingAcceptCount[l] = 0;
            samplingTotalCount[l] = 0;

            E[l] = replicaSet[l]->energy(*m_pDataSet, H[l]);
        }

//        for (unsigned i = 0; i < num; ++i)

        int i = 0, j = 0, k = 0;

        for(;;)
        {
            // if ((i % 100) == 0)
            //     printf("MCMC processing ... E= %f, n = %d\n", E, i);
            IMCMCParameter* pBParam = NULL;

            for (int l = i & 0x1; l < numOfReplica - 1; l += 2)
            {
                double r = exp((replicaSet[l + 1]->getTemperature() - replicaSet[l]->getTemperature()) * (H[l + 1] - H[l]));

                if (m_pRng->uniform() < r)
                {
                    replicaSet[l]->swap(replicaSet[l + 1]);

                    E[l] = replicaSet[l]->energy(*m_pDataSet, H[l]);
                    E[l + 1] = replicaSet[l + 1]->energy(*m_pDataSet, H[l + 1]);

                    ++exchangeAcceptCount[l];
                }
                ++exchangeTotalCount[l];
            }
        
            for (int l = 0; l < numOfReplica; ++l)
            {
//                 printf("l = %d\n", l);
                IMCMCParameter* pParam = replicaSet[l];

                pParam->next(i);
                double nextE = pParam->energy(*m_pDataSet, H[l]);
                double dE = nextE - E[l];

                bool isAccepted = (dE <= 0) || (m_pRng->uniform() < exp(-dE));

//                printf("replica = %d, energy = %f, next energy = %f\n", l, E[l], nextE);

                if (isAccepted)
                {
                    pParam->accept();
                    E[l] = nextE;

                    ++samplingAcceptCount[l];
                }
                else
                {
                    pParam->reject();
                }

                //if (pParam->getTemperature() == 1.0)
                if (l == 0)
                {
                    pBParam = pParam;
                }

                ++samplingTotalCount[l];
            }

//            printf("i = %d, j = %d, k = %d, r = %d\n", i, j, k, acceptNum);

            if (i++ >= skip)
            {
                if (++j < step)
                {
                    continue;
                }

                j = 0;

                pBParam->push(*m_pParamSet);
//                 printf("id = %x\n", (int)pBParam);

                if (++k >= num)
                {
                    break;
                }
            }


//        fprintf(fp, "%f %f\n", *(w1->data), *(w2->data));
        }

        for (int l = 0; l < numOfReplica; ++l)
        {
            printf("[%f] exchange ratio = %f, accept ratio = %f, last energy = %f\n", replicaSet[l]->getTemperature(),
                   (double)exchangeAcceptCount[l] / (double)exchangeTotalCount[l],
                   (double)samplingAcceptCount[l] / (double)samplingTotalCount[l],
                   E[l]);
        }

        printf("sampling done = %d, total = %d, initial skip = %d, step = %d\n", k, i, skip, step);
    }

    void printfPramSet(FILE* fp)
    {
        for (std::vector<IMCMCParameter*>::iterator i = m_pParamSet->begin(); i != m_pParamSet->end(); ++i)
        {
            (*i)->print(fp);
        }
    }

    ublas::vector<double> value(const ublas::vector<double>& x) const
    {
        ublas::vector<double> sum;

        for (std::vector<IMCMCParameter*>::iterator i = m_pParamSet->begin(); i != m_pParamSet->end(); ++i)
        {
            sum += (*i)->value(x);
        }

        return sum / m_pParamSet->size();
    }

    double WAIC()
    {
        double sumOfLogProbStar = 0.0;
        double sumOfLogProb = 0.0;

        for (std::vector<Data>::iterator i = m_pDataSet->begin(); i != m_pDataSet->end(); ++i)
        {
            double maxInnerProb = 0.0;
            
            for (std::vector<IMCMCParameter*>::iterator j = m_pParamSet->begin(); j != m_pParamSet->end(); ++j)
            {
                double pr =  prob(i->x, i->y, **j);
                
                sumOfLogProb += pr / (2 * m_sigma * m_sigma)
                    +log(pow(sqrt(2 * M_PI) * m_sigma, static_cast<double>(i->y.size())));
                
                if (maxInnerProb < pr)
                    maxInnerProb = pr;
            }

            double diff = 0.0;

            for (std::vector<IMCMCParameter*>::iterator j = m_pParamSet->begin(); j != m_pParamSet->end(); ++j)
            {
                double pr =  prob(i->x, i->y, **j);

                diff += exp((-pr + maxInnerProb) / (2 * m_sigma * m_sigma));
            }
            
            if (m_pParamSet->size() == 1)
            {
                sumOfLogProbStar += maxInnerProb / (2 * m_sigma * m_sigma)
                    + log(static_cast<double>(m_pParamSet->size()) * pow(sqrt(2 * M_PI) * m_sigma, static_cast<double>(i->y.size())));
            }
            else
            {
                sumOfLogProbStar += maxInnerProb / (2 * m_sigma * m_sigma)
                    + log(static_cast<double>(m_pParamSet->size()) * pow(sqrt(2 * M_PI) * m_sigma, static_cast<double>(i->y.size())))
                    - log(1.0 + diff);
            }
        }

        m_Gt = sumOfLogProb / (m_pParamSet->size() * static_cast<double>(m_pDataSet->size()));
        m_Bt = sumOfLogProbStar / static_cast<double>(m_pDataSet->size());

        return 2 * m_Gt - m_Bt;
    }

    double Gt()
    {
        return m_Gt;
    }

    double Bt()
    {
        return m_Bt;
    }

    void setSigma(double sigma) { m_sigma = sigma; }
    void setParameterSet(std::vector<IMCMCParameter*>& set) { m_pParamSet = &set; }
    void setTrueParameterSet(std::vector<IMCMCParameter*>& set) { m_pTrueParamSet = &set; }
    void setDataSet(std::vector<Data>& set) { m_pDataSet = &set; }

    double prob(const ublas::vector<double>& x, const ublas::vector<double>& y, const IParameter& w)
    {
        ublas::vector<double> r = y - w.value(x);

//         return exp(- inner_prod(r, r) / (2 * m_sigma * m_sigma))
//             / pow(sqrt(2 * M_PI) * m_sigma, static_cast<double>(y.size()));

        return inner_prod(r, r);;
    }
//private:

    RandomGenerator* m_pRng;
    double m_sigma;
    std::vector<IMCMCParameter*>* m_pParamSet;
    std::vector<IMCMCParameter*>* m_pTrueParamSet;
    std::vector<Data>* m_pDataSet;

    double m_Bt;
    double m_Gt;
};

#endif // BAYES_H_
