//
// Created by Vibhav Gogate on 9/22/19.
//

#ifndef CNPROPOSAL_GM_H
#define CNPROPOSAL_GM_H

#include <vector>
#include <string>
#include <Variable.h>
#include <Function.h>
#include <Data.h>
#include <SamplingFunction.h>
#include <algorithm>


struct GM {
    vector<Variable *> variables;
    vector<Function> functions;

    GM() {}

    ~GM() {}


    void addBinaryCPT(int from, int to);

    void addUnaryCPT(int i);

    void updateCPTs(Data &data, vector<ldouble> &weights);

    ldouble getProbability(vector<int>& example);
    ldouble getLogProbability(vector<int>& example);

    void readUAI08(const string filename);


};

#endif //CNPROPOSAL_GM_H
