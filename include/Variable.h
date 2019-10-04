//
// Created by Vibhav Gogate on 9/22/19.
//

#ifndef CNPROPOSAL_VARIABLE_H
#define CNPROPOSAL_VARIABLE_H

#include <MyTypes.h>
#include <iostream>
using namespace std;
struct Variable{
    // Id of the variable, value assigned, domain size and temporary value assigned
    int id,value,d,t_value;
    // Constructor
    Variable(): id(-1),value(-1),d(0),t_value(-1){}
    ~Variable(){}
    Variable(int id_,int d_):id(id_),d(d_),value(-1),t_value(-1){}
    inline void setValue(int value_){if(value_<d) {value=value_;} else {cerr<<"Wrong value assigned\n";exit(-1);}}
    inline int getValue(){return value;}
    inline bool isEvidence(){
        return (value!=INVALID_VALUE);
    }
};

#endif //CNPROPOSAL_VARIABLE_H
