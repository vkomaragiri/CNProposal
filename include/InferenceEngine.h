//
// Created by Vibhav Gogate on 9/27/19.
//

#ifndef CNPROPOSAL_INFERENCEENGINE_H
#define CNPROPOSAL_INFERENCEENGINE_H

#include <MyTypes.h>


class MarginalInferenceEngine{
public:
    // Get Probability of Evidence or Partition Function
    virtual ldouble getPE()=0;
    virtual void getVarMarginals(vector<vector<ldouble> >& var_marginals)=0;
};

template <class T>
class PosteriorDistCreator{
public:
    virtual void getPosteriorDist(T& dist)=0;
};

template <class T>
class PosteriorSamplerCreator{
public:
    virtual void getPosteriorSampler(T& sampler)=0;
};

#endif //CNPROPOSAL_INFERENCEENGINE_H
