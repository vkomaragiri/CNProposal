//
// Created by Vibhav Gogate on 9/23/19.
//

#ifndef CNPROPOSAL_SAMPLINGFUNCTION_H
#define CNPROPOSAL_SAMPLINGFUNCTION_H

#include <vector>
#include <random>
#include <iostream>
using namespace std;

#include <MyTypes.h>
#include <Variable.h>
#include <Function.h>
#include <Utils.h>
#include <myRandom.h>

struct SamplingFunction{
    vector<Variable*> other_variables;
    Variable* variable;
    vector<discrete_distribution<int> > distributions;
    SamplingFunction(Function& function, Variable* variable_): variable(variable_)
    {

        vector<Variable*> tmp_variables(1);
        tmp_variables[0]=variable;
        Utils::do_set_difference(function.variables,tmp_variables,other_variables);
        int num_values=Utils::getDomainSize(other_variables);
        distributions=vector<discrete_distribution<int> > (num_values);
        for(int i=0;i<num_values;i++){
            Utils::setAddress(other_variables,i);
            vector<ldouble> myarray(variable->d);
            for(int j=0;j<variable->d;j++) {
                variable->t_value=j;
                myarray[j]=function.table[Utils::getAddress(function.variables)];
            }
            Utils::normalize1d(myarray);
            distributions[i]=discrete_distribution<int>(myarray.begin(),myarray.end());
        }
        /*
        cout<<"Printing input function\n";
        function.print();
        cout<<"Printing distributions for "<<variable->id<<endl;
        for(int i=0;i<num_values;i++){
            for(auto j: distributions[i].probabilities()){
                cout<<j<<" ";
            }
            cout<<endl;
        }
        cout<<"End: print sampling function\n";
         */
    }

    inline ldouble generateSample(){

        /*
        cout<<variable->id<<"|";
        for(auto & other_variable : other_variables)
            cout<<other_variable->id<<" ";
        cout<<" : ";
        for (ldouble x:distributions[Utils::getAddress(other_variables)].probabilities())
            cout << x << " ";
        */

        /*extern myRandom my_random;
        ldouble sum=0;
        int value=0;
        double num=my_random.getDouble();

        for(auto v:distributions[Utils::getAddress(other_variables)].probabilities()){
            sum+=v;
            if(sum>=num) break;
            value++;
        }
        if(value>=variable->d) value=variable->d-1;
        */
        int address=Utils::getAddress(other_variables);
        int value= distributions[address](myRandom::m_g);
        variable->t_value=value;
        return  distributions[address].probabilities()[value];
    }
    void print(){
        cout<<variable->id<<"|";
        for(auto & other_variable : other_variables)
            cout<<other_variable->id<<" ";
        cout<<" : ";
        for(int i=0;i<distributions.size();i++){
            for(int j=0;j<distributions[i].probabilities().size();j++)
                cout<<distributions[i].probabilities()[j]<<" ";
            cout<<endl;
        }
    }

};
#endif //CNPROPOSAL_SAMPLINGFUNCTION_H
