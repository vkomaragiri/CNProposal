//
// Created by Vibhav Gogate on 9/27/19.
//

#ifndef CNPROPOSAL_MT_BTP_H
#define CNPROPOSAL_MT_BTP_H

#include <MT.h>
#include <MTSampler.h>
#include <BTP.h>
class MT_BTP:public MarginalInferenceEngine,PosteriorDistCreator<MT>,PosteriorSamplerCreator<MTSampler>{
private:
    MT* mt;
    vector<BTP> btps;
public:
    MT_BTP();
    explicit MT_BTP(MT& mt_);
    ~MT_BTP()= default;

    ldouble getPE() override;
    void getVarMarginals(vector<vector<ldouble> >& var_marginals) override;
    void getPosteriorDist(MT& mt) override;
    void getPosteriorSampler(MTSampler& mt_sampler) override;
};
#endif //CNPROPOSAL_MT_BTP_H
