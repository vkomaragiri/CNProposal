//
// Created by Vibhav Gogate on 9/23/19.
//

#ifndef CNPROPOSAL_MT_H
#define CNPROPOSAL_MT_H

#include <vector>
#include <iostream>
using namespace std;
#include <Variable.h>
#include <Function.h>
#include <GM.h>
#include <Utils.h>
#include <MyTypes.h>
#include <Data.h>
#include <HyperParameters.h>

struct MT
{
public:
    int ncomponents;
    vector<Variable*> variables;
    vector<ldouble> probabilities;
    vector<GM> tree_gms;
    MT():ncomponents(0){}
    ~MT(){}
    void learn(Data& data);
    // Ger Posterior by Performing Inference along the min degree order
    void write(const string& filename);
    void read(const string& filename);
    ldouble getProbability(vector<int>& example);
    ldouble getLogProbability(vector<int>& example);
};
#endif //CNPROPOSAL_MT_H
