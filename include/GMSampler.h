//
// Created by Vibhav Gogate on 9/24/19.
//

#ifndef CNPROPOSAL_GMSAMPLER_H
#define CNPROPOSAL_GMSAMPLER_H

#include <vector>
#include <string>
#include <Utils.h>
#include <Variable.h>
#include <SamplingFunction.h>
#include <Function.h>
#include <GM.h>

using namespace std;
struct GMSampler{
    vector<int> evidence_vars;
    vector<Variable*> variables;
    vector<SamplingFunction> sampling_functions;
    GMSampler()= default;
    explicit GMSampler(GM& gm):variables(gm.variables)
    {
        vector<int> order;
        Utils::getTopologicalOrder(gm.variables,gm.functions,order);
        vector<int> var_func(order.size());
        for(int i=0;i<gm.functions.size();i++)
            var_func[gm.functions[i].cpt_var->id]=i;
        for(int i=0;i<order.size();i++){
            int var=order[i];
            sampling_functions.emplace_back(gm.functions[var_func[var]],variables[var]);
        }
    }
    GMSampler(vector<Variable*>& variables_, vector<SamplingFunction>& sampling_functions_):
    variables(variables_),sampling_functions(sampling_functions_)
    {
        evidence_vars=vector<int>();
        for(auto & variable : variables){
            if(variable->value!=INVALID_VALUE)
                evidence_vars.push_back(variable->id);
        }
    }
    void setEvidenceVariables()
    {
        evidence_vars=vector<int>();
        for(auto & variable : variables){
            if(variable->isEvidence())
                evidence_vars.push_back(variable->id);
        }
    }
    void generateSamples(int n, vector<vector<int> >&assignments)
    {
        assignments=vector<vector<int> >(n);
        for(int i=0;i<n;i++)
        {
            assignments[i]=vector<int>(variables.size());
            for(auto & sampling_function : sampling_functions) {
                sampling_function.generateSample();
                assignments[i][sampling_function.variable->id] =sampling_function.variable->t_value;
            }
            for(int j : evidence_vars){
                assignments[i][j]=variables[j]->value;
            }
        }
    }
    ldouble getProbability(vector<int>& assignment){
        ldouble p=1.0;
        for(int i=0;i<assignment.size();i++){
            if (variables[i]->isEvidence()){
                variables[i]->t_value=variables[i]->value;
            }
            else{
                variables[i]->t_value=assignment[i];
            }
        }
        for (auto& s: sampling_functions) {
            if (s.variable->isEvidence()) continue;
            int a=Utils::getAddress(s.other_variables);
            int v=s.variable->t_value;
            p*=s.distributions[a].probabilities()[v];
        }
        return p;
    }
};
#endif //CNPROPOSAL_GMSAMPLER_H
