//
// Created by Vibhav Gogate on 9/22/19.
//

#include <iostream>

#include <Function.h>
#include <Utils.h>

Function::Function(vector<Variable *> &variables_): variables(variables_), cpt_var(nullptr) {
    table = vector<ldouble>(Utils::getDomainSize(variables), 1.0);
}

Function::Function(vector<Variable *> &variables_, vector<ldouble> &table_) : variables(variables_), table(table_),
                                                                              cpt_var(nullptr) {
}

// Creates a new function out after instantiating evidence
// The scope of the new function is variables "set minus" evidence_variables
// Table = original table projected on the new scope
void Function::instantiateEvidence(Function &out) {
    out = Function();

    // Base case if the variables are empty, return
    if (variables.empty()) return;

    // Set the scope of the new function is variables "set minus" evidence_variables
    for (auto &variable : variables) {
        if (variable->isEvidence()) {
            // Set appropriate values for evidence variables
            variable->t_value = variable->value;
        } else {
            // Add non evidence variables to the set of variables
            out.variables.push_back(variable);
        }
    }
    // If no variables in the original are evidence variables
    if (out.variables.size() == variables.size()) {
        out.cpt_var=cpt_var;
        out.table = table;
    }
    else {
        int dom_size = Utils::getDomainSize(out.variables);
        out.table = vector<ldouble>(dom_size,1.0);
        for (int i = 0; i < dom_size; i++) {
            Utils::setAddress(out.variables, i);
            out.table[i] = table[Utils::getAddress(variables)];
        }
    }
}

void Function::print() {
    cout << "Variables: ";
    for (auto &variable : variables) {
        cout << variable->id << " ";
    }
    if(cpt_var!= nullptr){
        cout<<", cpt-var = "<<cpt_var->id;
    }
    cout << endl;
    cout << "Table: ";
    for (ldouble i : table) {
        cout << i << " ";
    }
    cout << endl;
}

