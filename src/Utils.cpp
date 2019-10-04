//
// Created by Vibhav Gogate on 9/22/19.
//

#include <vector>
#include <random>
#include <algorithm>
#include <set>
#include <Utils.h>

using namespace std;

std::random_device rd;
ranlux48 Utils::rand_generator(rd());

int Utils::getDomainSize(vector<Variable *> &variables) {
    int all_d = 1;
    for (auto &variable : variables)
        all_d *= variable->d;
    return all_d;
}

bool Utils::less_than_comparator_variable(const Variable *a, const Variable *b) {
    return (a->id < b->id);
}

void Utils::do_set_difference(vector<Variable *> &a, vector<Variable *> &b, vector<Variable *> &out) {
    out = vector<Variable *>(a.size());
    vector<Variable *>::iterator it;
    it = set_difference(a.begin(), a.end(), b.begin(), b.end(), out.begin(), Utils::less_than_comparator_variable);
    out.resize(it - out.begin());
}

void Utils::do_set_intersection(vector<Variable *> &a, vector<Variable *> &b, vector<Variable *> &out) {
    out = vector<Variable *>(a.size());
    vector<Variable *>::iterator it;
    it = set_intersection(a.begin(), a.end(), b.begin(), b.end(), out.begin(), Utils::less_than_comparator_variable);
    out.resize(it - out.begin());
}

void Utils::do_set_union(vector<Variable *> &a, vector<Variable *> &b, vector<Variable *> &out) {
    out = vector<Variable *>(a.size() + b.size());
    vector<Variable *>::iterator it;
    it = set_union(a.begin(), a.end(), b.begin(), b.end(), out.begin(), Utils::less_than_comparator_variable);
    out.resize(it - out.begin());
}

int Utils::getAddress(vector<Variable *> &vars) {
    int address = 0, multiplier = 1;
    for (int i = vars.size() - 1; i > -1; i--) {
        address += vars[i]->t_value * multiplier;
        multiplier *= vars[i]->d;
    }
    return address;
}

void Utils::setAddress(vector<Variable *> &vars, int address_) {
    int address = address_;
    for (int i = vars.size() - 1; i > -1; i--) {
        vars[i]->t_value = address % vars[i]->d;
        address /= vars[i]->d;
    }
}

ldouble Utils::multiplyAndMarginalize(vector<Function> &functions, vector<Variable *> &marg_variables,
                                      Function &outf, bool normalize) {

    vector<Variable *> all_vars = marg_variables;
    for (auto &f : functions) {
        vector<Variable *> tmp_vars;
        Utils::do_set_union(all_vars, f.variables, tmp_vars);
        all_vars = tmp_vars;
    }
    vector<Variable *> marg_out_vars;
    do_set_difference(all_vars, marg_variables, marg_out_vars);
    outf = Function(marg_variables);
    outf.table=vector<ldouble>(Utils::getDomainSize(marg_variables),0.0);
    int all_values = Utils::getDomainSize(all_vars);
    for (int i = 0; i < all_values; i++) {
        ldouble prod = 1.0;
        Utils::setAddress(all_vars, i);
        for (auto &f : functions) {
            int address = Utils::getAddress(f.variables);
            prod *= f.table[address];
        }
        outf.table[Utils::getAddress(outf.variables)] += prod;
    }
    if (normalize) {
        Utils::normalizeAndGetNormConst(outf);
    }
    if (marg_variables.empty()) {
        return outf.table[0];
    }
    return -1.0;
}

ldouble Utils::multiplyAndMarginalize(vector<Function *> &functions, vector<Variable *> &marg_variables,
                                      Function &outf, bool normalize) {

    vector<Variable *> all_vars = marg_variables;
    for (auto &f : functions) {
        vector<Variable *> tmp_vars;
        Utils::do_set_union(all_vars, f->variables, tmp_vars);
        all_vars = tmp_vars;
    }
    vector<Variable *> marg_out_vars;
    do_set_difference(all_vars, marg_variables, marg_out_vars);
    outf = Function(marg_variables);
    outf.table=vector<ldouble>(Utils::getDomainSize(marg_variables),0.0);
    int all_values = Utils::getDomainSize(all_vars);
    for (int i = 0; i < all_values; i++) {
        ldouble prod = 1.0;
        Utils::setAddress(all_vars, i);
        for (auto &f : functions) {
            int address = Utils::getAddress(f->variables);
            prod *= f->table[address];
        }
        outf.table[Utils::getAddress(outf.variables)] += prod;
    }
    if (normalize) {
        Utils::normalizeAndGetNormConst(outf);
    }
    if (marg_variables.empty()) {
        return outf.table[0];
    }
    return -1.0;
}

ldouble Utils::normalizeAndGetNormConst(Function &function) {
    ldouble norm_const = 0.0;
    for (ldouble i : function.table) {
        norm_const += i;
    }
    for (ldouble & i : function.table) {
        i /= norm_const;
    }
    return norm_const;
}

void Utils::functionToCPT(Function &function, Variable *marg_variable) {
    int num_values = Utils::getDomainSize(function.variables);
    num_values /= marg_variable->d;
    vector<Variable *> other_variables;
    vector<Variable *> tmp_variables;
    tmp_variables.push_back(marg_variable);
    do_set_difference(function.variables, tmp_variables, other_variables);

    for (int i = 0; i < num_values; i++) {
        Utils::setAddress(other_variables, i);
        ldouble norm_const = 0.0;
        for (int j = 0; j < marg_variable->d; j++) {
            marg_variable->t_value = j;
            norm_const += function.table[Utils::getAddress(function.variables)];
        }
        for (int j = 0; j < marg_variable->d; j++) {
            marg_variable->t_value = j;
            function.table[Utils::getAddress(function.variables)] /= norm_const;
        }
    }
}

void Utils::getMinDegreeOrder(vector<Variable *> &variables, vector<Function> &functions, vector<int> &order) {
    vector<set<int> > degree(variables.size());
    vector<set<int> > graph(variables.size());

    for (auto &function : functions) {
        for (int j = 0; j < function.variables.size(); j++) {
            if (function.variables[j]->isEvidence()) continue;
            for (int k = j + 1; k < function.variables.size(); k++) {
                if (function.variables[k]->isEvidence()) continue;
                int a = function.variables[j]->id;
                int b = function.variables[k]->id;
                graph[a].insert(b);
                graph[b].insert(a);
            }
        }
    }
    int min_degree = variables.size();
    int max_degree = -1;
    for (int i = 0; i < variables.size(); i++) {
        degree[graph[i].size()].insert(i);
        if (graph[i].size() < min_degree) {
            min_degree = graph[i].size();
        }
        if (graph.size() > max_degree) {
            max_degree = graph.size();
        }
    }
    order = vector<int>();
    while (order.size() != variables.size()) {
        while (degree[min_degree].empty()) {
            min_degree++;
        }
        int curr = *(degree[min_degree].begin());
        degree[min_degree].erase(curr);
        for (auto i = graph[curr].begin(); i != graph[curr].end(); i++) {
            degree[graph[*i].size()].erase(*i);
            graph[*i].erase(curr);
            degree[graph[*i].size()].insert(*i);
            if (graph[*i].size() < min_degree) { min_degree = graph[*i].size(); }
        }
        order.push_back(curr);
    }
}

void Utils::getTopologicalOrder(vector<Variable *> &variables, vector<Function> &functions, vector<int> &order) {
    vector<set<int> > degree(variables.size());
    vector<set<int> > parents(variables.size());
    vector<set<int> > children(variables.size());

    for (auto &function : functions) {
        if(function.cpt_var==nullptr){
            cerr<<"Something wrong: Topological order for a non Bayes net\n";
            exit(-1);
        }
        int a=function.cpt_var->id;
        for (int j = 0; j < function.variables.size(); j++) {
                if (function.variables[j]->id==function.cpt_var->id) continue;
                int b = function.variables[j]->id;
                parents[a].insert(b);
                children[b].insert(a);
            }
    }
    for (int i = 0; i < variables.size(); i++) {
        degree[parents[i].size()].insert(i);
    }
    order = vector<int>();
    while (!degree[0].empty()) {
        int curr = *(degree[0].begin());
        degree[0].erase(curr);
        for (auto i = children[curr].begin(); i != children[curr].end(); i++) {
            degree[parents[*i].size()].erase(*i);
            parents[*i].erase(curr);
            degree[parents[*i].size()].insert(*i);
        }
        order.push_back(curr);
    }
    if(order.size()!=variables.size()){
        cerr<<"Something wrong while computing topological order\n";
        cerr<<"Seems that the Bayes net is not a DAG\n";
        exit(-1);
    }
}