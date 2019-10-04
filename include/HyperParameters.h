//
// Created by Vibhav Gogate on 9/23/19.
//

#ifndef CNPROPOSAL_HYPERPARAMETERS_H
#define CNPROPOSAL_HYPERPARAMETERS_H

#include <MyTypes.h>
struct HyperParameters{
    static int num_iterations_em;
    static int max_height;
    static bool prune;
    static bool tune;
    static bool merge;
    static bool run_mixture;
    static ldouble tol;
    static int num_components;
    static int interval_for_structure_learning;
    static ldouble  mi_low_limit;
    HyperParameters()= default;
    static void setNumIterationsEM(int x){if (x<100000) num_iterations_em=x;}
    static void setIntervalForStructureLearning(int x){interval_for_structure_learning=x;}
    static void setNumComponents(int x){num_components=x;};
    static void setTolerance(ldouble x){tol=x;}
};
#endif //CNPROPOSAL_HYPERPARAMETERS_H
