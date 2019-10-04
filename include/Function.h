//
// Created by Vibhav Gogate on 9/22/19.
//

#ifndef CNPROPOSAL_FUNCTION_H
#define CNPROPOSAL_FUNCTION_H
// Standard includes

#include <vector>

using namespace std;
// Project specific includes
#include <MyTypes.h>
#include <Variable.h>

struct Function {
    /* Member Variables*/

    // Scope of the function
    // Namely, all variables involved in the function (including the CPT variable).
    // The variables are always assumed to be sorted
    vector<Variable *> variables;
    // The following variable is set only when the function is a CPT
    Variable *cpt_var;
    // 1-D table storing all possible combinations of value assignments to the variables
    // Note: if the set of variables is empty, table will have one entry = 1.0
    vector<ldouble> table;


    /* Constructors and Destructors */
    Function() : table(vector<ldouble>(1, 1.0)), cpt_var(nullptr) {}

    Function(vector<Variable *> &variables_);

    Function(vector<Variable *> &variables_, vector<ldouble> &table_);

    ~Function() {
        variables.clear();
        table.clear();
        variables.shrink_to_fit();
        table.shrink_to_fit();
    }

    /* Other Member Functions */

    // Creates a new function out after instantiating evidence
    // The scope of the new function is variables "set minus" evidence_variables
    // Table = original table projected on the new scope
    void instantiateEvidence(Function &out);

    void print();
};


#endif //CNPROPOSAL_FUNCTION_H
