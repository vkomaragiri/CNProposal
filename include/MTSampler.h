//
// Created by Vibhav Gogate on 9/24/19.
//

#ifndef CNPROPOSAL_MTSAMPLER_H
#define CNPROPOSAL_MTSAMPLER_H
#include <vector>
#include <GM.h>
#include <Utils.h>
#include <Variable.h>
#include <SamplingFunction.h>
#include <Function.h>
#include <GMSampler.h>
#include <MT.h>

using namespace std;
struct MTSampler{
    discrete_distribution<int>  mixture_distribution;
    vector<Variable*> variables;
    vector<GMSampler> samplers;
    MTSampler()= default;
    MTSampler(MT& mt){
        variables=mt.variables;
        for(int i=0;i<mt.ncomponents;i++){
            samplers.emplace_back(mt.tree_gms[i]);
        }
        mixture_distribution = discrete_distribution<int>(mt.probabilities.begin(), mt.probabilities.end());
    }
    void generateSamples(int n, vector<vector<int> >&assignments)
    {

        assignments=vector<vector<int> >();

        vector<int> mixcount(samplers.size(),0);
        for(int i=0;i<n;i++) {
            int c = mixture_distribution(Utils::rand_generator);
            mixcount[c]++;
        }
        cout<<"Mixture probs\n";
        for(auto i: mixture_distribution.probabilities()){
            cout<<i<<" ";
        }
        cout<<endl;
        cout<<"Mixture counts = ";
        for(int i=0;i<samplers.size();i++){
            cout<<mixcount[i]<<" ";
            if(mixcount[i]>0)
            {
                vector<vector<int> > tmp_assignments;
                samplers[i].generateSamples(mixcount[i],tmp_assignments);
                assignments.insert(assignments.end(),tmp_assignments.begin(),tmp_assignments.end());

            }
        }
        cout<<endl;
    }
    void generateSamples(int n, vector<vector<int> >&assignments, vector<int>& mixture_assignments)
    {

        assignments=vector<vector<int> >();
        mixture_assignments=vector<int>();
        vector<int> mixcount(samplers.size(),0);
        for(int i=0;i<n;i++) {
            int c = mixture_distribution(Utils::rand_generator);
            mixcount[c]++;
        }
        cout<<"Mixture probs\n";
        for(auto i: mixture_distribution.probabilities()){
            cout<<i<<" ";
        }
        cout<<endl;
        cout<<"Mixture counts = ";
        for(int i=0;i<samplers.size();i++){
            cout<<mixcount[i]<<" ";
            if(mixcount[i]>0)
            {
                vector<vector<int> > tmp_assignments;
                samplers[i].generateSamples(mixcount[i],tmp_assignments);
                assignments.insert(assignments.end(),tmp_assignments.begin(),tmp_assignments.end());
                mixture_assignments.insert(mixture_assignments.end(),mixcount[i],i);
            }
        }
        cout<<endl;
    }
    ldouble getProbability(vector<int>& assignment)
    {
        ldouble p=0.0;
        for(int i=0;i<samplers.size();i++){
            p+=mixture_distribution.probabilities()[i]*samplers[i].getProbability(assignment);
        }
        return p;
    }
};
#endif //CNPROPOSAL_MTSAMPLER_H
