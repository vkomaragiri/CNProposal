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
    /*
    GM(const GM &gm) {
        for(auto var: gm.variables){
            Variable* var2 = new Variable(*(var));
            variables.push_back(var2);
        }
        for(auto func: gm.functions){
            functions.push_back(Function(func));
        }

    }
    */
    ~GM() {}


    void addBinaryCPT(int from, int to);

    void addUnaryCPT(int i);

    void updateCPTs(Data &data, vector<ldouble> &weights);

    ldouble getProbability(vector<int>& example);
    ldouble getLogProbability(vector<int>& example);

    void updateParams(Data &d);

    void readUAI08(const string filename);
    void write(const string& filename);

};

#endif //CNPROPOSAL_GM_H
