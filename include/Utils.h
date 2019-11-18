//
// Created by Vibhav Gogate on 9/22/19.
//

#ifndef CNPROPOSAL_UTILS_H
#define CNPROPOSAL_UTILS_H

#include <vector>
#include <iostream>
#include <random>
#include <cmath>
#include <iomanip>
#include <fstream>
using namespace std;

#include <MyTypes.h>
#include <Variable.h>
#include <Function.h>
#include <Data.h>

struct Utils{
    static int getDomainSize(vector<Variable*>& variables);
    static bool less_than_comparator_variable(const Variable* a, const Variable* b);
    static void do_set_intersection(vector<Variable*>& a, vector<Variable*>& b, vector<Variable*>& out);
    static void do_set_union(vector<Variable*>& a, vector<Variable*>& b, vector<Variable*>& out);
    static void do_set_difference(vector<Variable*>& a, vector<Variable*>& b, vector<Variable*>& out);
    static int getAddress(vector<Variable*>& vars);
    static void setAddress(vector<Variable*>& vars, int address);
    static ldouble multiplyAndMarginalize(vector<Function>& functions, vector<Variable*>& marg_variables, Function& function,bool normalize=true);
    static ldouble multiplyAndMarginalize(vector<Function*>& functions, vector<Variable*>& marg_variables, Function& function,bool normalize=true);
    static ldouble normalizeAndGetNormConst(Function& function);
    static void functionToCPT(Function& function, Variable* marg_variable);


    template <class T>
    static void normalize1d(vector<T>&v){
        T norm_const=0.0;
        for(int i=0;i<v.size();i++)
            norm_const+=v[i];
        for(int i=0;i<v.size();i++)
            v[i]/=norm_const;
    }
    template <class T>
    static void normalize2d(vector<vector<T> >&v){
        T norm_const=0.0;
        for(int i=0;i<v.size();i++)
            for(int j=0;j<v[i].size();j++)
                norm_const+=v[i][j];
        for(int i=0;i<v.size();i++)
            for(int j=0;j<v[i].size();j++)
                v[i][j]/=norm_const;
    }
    template <class T>
    static T sum1d(vector<T> &v){
        T norm_const=0.0;
        for(int i=0;i<v.size();i++)
            norm_const+=v[i];
        return norm_const;
    }
    template <class T>
    static void normalizeDim2(vector<vector<T> >&v){
        if(v.empty()) return;
        vector<T> norm_const(v[0].size(),0.0);
        for(int i=0;i<v.size();i++)
            for(int j=0;j<v[i].size();j++)
                norm_const[j]+=v[i][j];
        for(int i=0;i<v.size();i++)
            for(int j=0;j<v[i].size();j++)
                v[i][j]/=norm_const[j];
    }
    template <class T>
    static ldouble getLLScore(Data& data, T& model)
    {
        ldouble p=0.0;
        for(int i=0;i<data.nexamples;i++){
            p+=log(model.getProbability(data.data_matrix[i]));
        }
        return p/(ldouble)data.nexamples;
    }
    template <class T>
    static void printVector(vector<T>& t, string print_me="")
    {
        //cout<<print_me<<endl;
        for(int i=0;i<t.size();i++){
            cout<<t[i]<<" ";
        }
        //cout<<endl;
    }
    static void getXYStat(Variable* x, Variable* y, Data& data, vector<ldouble>& weights, vector<vector<ldouble> >& stats)
    {
        stats=vector<vector<ldouble> > (x->d,vector<ldouble>(y->d,1.0));
        vector<vector<int> >& examples=data.data_matrix;
        for(int i=0;i<data.nexamples;i++){
            stats[examples[i][x->id]][examples[i][y->id]]+=weights[i];
        }
    }
    static void getXStat(Variable* x, Data& data, vector<ldouble>& weights, vector<ldouble>& stats)
    {
        stats=vector<ldouble>(x->d,2.0);
        vector<vector<int> >& examples=data.data_matrix;
        for(int i=0;i<data.nexamples;i++){
            stats[examples[i][x->id]]+=weights[i];
        }
    }
    static void getOrder(vector<Variable*>& variables, vector<Function>& functions, vector<int>& order);
    static void getMinDegreeOrder(vector<Variable*>& variables, vector<Function>& functions, vector<int>& order);
    static void getTopologicalOrder(vector<Variable*>& variables, vector<Function>& functions, vector<int>& order);
    static void getMinFillOrder(vector<Variable*>& variables, vector<Function>& functions, vector<int>& order);
    static ldouble computeAbsError(vector<vector<ldouble> >& v1, vector<vector<ldouble> >& v2)
    {
        if(v1.size()==v2.size()){
            ldouble err=0.0;
            for(int i=0;i<v1.size();i++){
                if(v1[i].size()==v2[i].size()){
                    for(int j=0;j<v1[i].size();j++)
                        err+=abs(v1[i][j]-v2[i][j]);
                    //Utils::printVector(v1[i],"Ex");
                    //Utils::printVector(v2[i],"App");
                }
                else
                {
                    return INVALID_VALUE;
                }
            }
            return err;
        }
        return INVALID_VALUE;
    }
    static ldouble computeHellingerError(vector<vector<ldouble> >& v1, vector<vector<ldouble> >& v2)
    {
        if(v1.size()==v2.size()){
            ldouble err=0.0;
            for(int i=0;i<v1.size();i++){
                if(v1[i].size()==v2[i].size()){
                    ldouble merr=0.0;
                    for(int j=0;j<v1[i].size();j++)
                        merr+=(sqrt(v1[i][j])-sqrt(v2[i][j]))*(sqrt(v1[i][j])-sqrt(v2[i][j]));
                    err+=sqrt(merr)/sqrt(2.0);
                    //Utils::printVector(v1[i],"Ex");
                    //Utils::printVector(v2[i],"App");
                }
                else
                {
                    return INVALID_VALUE;
                }
            }
            return err/=(ldouble)v1.size();
        }
        return INVALID_VALUE;
    }
    static void printMarginals(vector<vector<ldouble> >& var_marginals, const string& outfilename)
    {
        ofstream out(outfilename);
        out<<"MAR\n";
        out<<var_marginals.size();
        for(int i=0;i<var_marginals.size();i++){
            out<<" "<<var_marginals[i].size();
            for(int j=0;j<var_marginals[i].size();j++)
                out<<" "<<var_marginals[i][j];
        }
        out<<"\n";
        out.close();
    }
};
#endif //CNPROPOSAL_UTILS_H
