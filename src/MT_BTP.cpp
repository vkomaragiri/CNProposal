//
// Created by Vibhav Gogate on 9/28/19.
//

#include <MT_BTP.h>

MT_BTP::MT_BTP():mt(nullptr){}

MT_BTP::MT_BTP(MT &mt_): mt(&mt_)
{
    for(int i=0;i<mt->ncomponents;i++){
        btps.emplace_back(mt->tree_gms[i]);
    }
}

ldouble MT_BTP::getPE()
{
    ldouble pe = 0.0;
    for (int i = 0; i <mt->ncomponents; i++) {
        pe+= mt->probabilities[i] * btps[i].getPE();
    }
   return pe;
}

void MT_BTP::getPosteriorDist(MT &other_mt) {
    other_mt = MT();
    other_mt.ncomponents = mt->ncomponents;
    other_mt.variables = mt->variables;
    other_mt.probabilities = vector<ldouble>(mt->ncomponents);
    other_mt.tree_gms = vector<GM>(mt->ncomponents);

    for (int i = 0; i < mt->ncomponents; i++) {
        btps[i].getPosteriorDist(other_mt.tree_gms[i]);
        other_mt.probabilities[i] = mt->probabilities[i] * btps[i].getPE();
    }
    Utils::normalize1d(mt->probabilities);
}

void MT_BTP::getPosteriorSampler(MTSampler &mtSampler) {
    vector<ldouble> post_probabilities(mt->ncomponents);
    mtSampler.variables = mt->variables;
    mtSampler.samplers = vector<GMSampler>(mt->ncomponents);
    for (int i = 0; i < mt->ncomponents; i++) {
        btps[i].getPosteriorSampler(mtSampler.samplers[i]);
        post_probabilities[i] = mt->probabilities[i] * btps[i].getPE();
    }
    Utils::normalize1d(post_probabilities);
    mtSampler.mixture_distribution = discrete_distribution<int>(post_probabilities.begin(), post_probabilities.end());
}

void MT_BTP::getVarMarginals(vector<vector<ldouble> > &var_marginals) {
    vector<ldouble> post_probabilities(mt->ncomponents);
    var_marginals=vector<vector<ldouble> >(mt->variables.size());
    for(int i=0;i<var_marginals.size();i++){
        var_marginals[i]=vector<ldouble>(mt->variables[i]->d,0.0);
    }
    for (int i = 0; i < mt->ncomponents; i++) {
        vector<vector<ldouble > > tmp_marginals;
        btps[i].getVarMarginals(tmp_marginals);
        post_probabilities[i] = mt->probabilities[i] * btps[i].getPE();
        for(int j=0;j<mt->variables.size();j++){
            for(int k=0;k<mt->variables[j]->d;k++)
                var_marginals[j][k]+=tmp_marginals[j][k]*post_probabilities[i];
        }
    }
    for(auto & var_marginal : var_marginals){
        Utils::normalize1d(var_marginal);
    }
}