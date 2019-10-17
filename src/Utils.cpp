//
// Created by Vibhav Gogate on 9/22/19.
//

#include <vector>
#include <random>
#include <algorithm>
#include <set>
#include <list>
#include <cfloat>
#include <Utils.h>
#include <myRandom.h>
#include <HyperParameters.h>

using namespace std;



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

void Utils::getMinFillOrder(vector<Variable *> &variables, vector<Function> &functions, vector<int> &order) {
    double estimate = 0.0;
    int max_cluster_size = 0;
    order = vector<int>(variables.size());
    vector<set<int> >clusters(variables.size());
    vector<vector<bool> > adj_matrix(variables.size());

    // Create the interaction graph of the functions in this graphical model - i.e.
    // create a graph structure such that an edge is drawn between variables in the
    // model that appear in the same function
    for (int i = 0; i < variables.size(); i++) {
        adj_matrix[i] = vector<bool>(variables.size());
    }
    vector<set<int> > graph(variables.size());
    vector<bool> processed(variables.size());
    for (auto & function : functions) {
        for (int j = 0; j < function.variables.size(); j++) {
            if (function.variables[j]->isEvidence()) continue;
            for (int k = j + 1; k < function.variables.size(); k++) {
                if (function.variables[k]->isEvidence()) continue;
                int a = function.variables[j]->id;
                int b = function.variables[k]->id;
                graph[a].insert(b);
                graph[b].insert(a);
                adj_matrix[a][b] = true;
                adj_matrix[b][a] = true;
            }
        }
    }
    list<int> zero_list;

    // For i = 1 to number of variables in the model
    // 1) Identify the variables that if deleted would add the fewest number of edges to the
    //    interaction graph
    // 2) Choose a variable, pi(i), from among this set
    // 3) Add an edge between every pair of non-adjacent neighbors of pi(i)
    // 4) Delete pi(i) from the interaction graph
    for (int i = 0; i < variables.size(); i++) {
        // Find variables with the minimum number of edges added
        double min = DBL_MAX;
        int min_id = -1;
        bool first = true;

        // Flag indicating whether the variable to be removed is from the
        // zero list - i.e. adds no edges to interaction graph when deleted
        bool fromZeroList = false;
        // Vector to keep track of the ID of each minimum fill variable
        vector<int> minFillIDs;

        // If there are no variables that, when deleted, add no edges...
        if (zero_list.empty()) {

            // For each unprocessed (non-deleted) variable
            for (int j = 0; j < variables.size(); j++) {
                if (processed[j])
                    continue;
                double curr_min = 0.0;
                for (auto a = graph[j].begin();
                     a != graph[j].end(); a++) {
                    auto b = a;
                    b++;
                    for (; b != graph[j].end(); b++) {
                        if (!adj_matrix[*a][*b]) {
                            curr_min += (variables[*a]->d
                                         * variables[*b]->d);
                            if (curr_min > min)
                                break;
                        }
                    }
                    if (curr_min > min)
                        break;
                }

                // Store the first non-deleted variable as a potential minimum
                if (first) {
                    minFillIDs.push_back(j);
                    min = curr_min;
                    first = false;
                } else {
                    // If this is a new minimum...
                    if (min > curr_min) {
                        min = curr_min;
                        minFillIDs.clear();
                        minFillIDs.push_back(j);
                    }
                        // Otherwise, if the number of edges removed is also a minimum, but
                        // the minimum is zero
                    else if (curr_min < DBL_MIN) {
                        zero_list.push_back(j);
                    }
                        // Else if this is another potential min_fill
                    else if (min == curr_min) {
                        minFillIDs.push_back(j);
                    }
                }
            }
        }
            // Else...delete variables from graph that don't add any edges
        else {
            min_id = zero_list.front();
            zero_list.pop_front();
            fromZeroList = true;
        }

        // If not from zero_list, choose one of the variables at random
        // from the set of min fill variables
        if (!fromZeroList) {
            int indexInVector;
            indexInVector = myRandom::getInt(minFillIDs.size());
            min_id = minFillIDs[indexInVector];
        }

        //cout<<"order["<<i<<"]= "<<min_id<<" "<<flush;
        assert(min_id!=-1);
        order[i] = min_id;
        // Now form the cluster
        clusters[i] = graph[min_id];
        clusters[i].insert(min_id);

        // Trinagulate min id and remove it from the graph
        for (auto a = graph[min_id].begin();
             a != graph[min_id].end(); a++) {
            auto b = a;
            b++;
            for (; b != graph[min_id].end(); b++) {
                if (!adj_matrix[*a][*b]) {
                    adj_matrix[*a][*b] = true;
                    adj_matrix[*b][*a] = true;
                    graph[*a].insert(*b);
                    graph[*b].insert(*a);
                }
            }
        }
        for (auto a = graph[min_id].begin();
             a != graph[min_id].end(); a++) {
            graph[*a].erase(min_id);
            adj_matrix[*a][min_id] = false;
            adj_matrix[min_id][*a] = false;
        }
        graph[min_id].clear();
        processed[min_id] = true;
    }

    // compute the estimate
    for (auto & cluster : clusters) {
        if ((int) cluster.size() > max_cluster_size)
            max_cluster_size = (int) cluster.size();
        double curr_estimate = 1.0;
        for (std::__1::__tree_const_iterator<int, std::__1::__tree_node<int, void *> *, long>::value_type j : cluster) {
            curr_estimate *= (double) variables[j]->d;
        }
        estimate += curr_estimate;
    }
    cout<<"Max cluster size = "<<max_cluster_size<<endl;
}

void Utils::getOrder(vector<Variable *> &variables, vector<Function> &functions, vector<int> &order)
{
    if (HyperParameters::ord_heu==min_fill){
        Utils::getMinFillOrder(variables,functions,order);
    } else{
        if (HyperParameters::ord_heu==min_degree){
            Utils::getMinDegreeOrder(variables,functions,order);
        }
        else {
            if(HyperParameters::ord_heu==topological){
                Utils::getTopologicalOrder(variables,functions,order);
            }
        }
    }
}