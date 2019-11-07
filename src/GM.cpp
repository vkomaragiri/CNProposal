//
// Created by Vibhav Gogate on 9/22/19.
//
#include <GM.h>
#include <Utils.h>


/*
ldouble GM::getPosteriorGM(GM& gm)
{
    BTP btp;
    btp.init(*this);
    ldouble pe=btp.downwardPass();
    btp.upwardPass();
    btp.getPosteriorGM(gm);
    return pe;
}
ldouble GM::getPosteriorGMSampler(GMSampler& gm_sampler)
{
    BTP btp;
    btp.init(*this);
    ldouble pe=btp.downwardPass();
    btp.upwardPass();
    btp.getPosteriorGMSampler(gm_sampler);
    return pe;
}
ldouble GM::getPosteriorMarginals(vector<vector<ldouble> >& var_marginals) {
    BTP btp;
    btp.init(*this);
    ldouble pe=btp.downwardPass();
    btp.upwardPass();
    btp.getVarMarginals(var_marginals);
    return pe;
}

*/
void GM::addBinaryCPT(int from, int to) {
    Function function;
    function.table = vector<ldouble>(variables[from]->d * variables[to]->d);
    if (from > to) {
        function.variables.push_back(variables[to]);
        function.variables.push_back(variables[from]);
    } else {
        function.variables.push_back(variables[from]);
        function.variables.push_back(variables[to]);
    }
    function.cpt_var = variables[to];
        functions.push_back(function);
}

void GM::addUnaryCPT(int i) {
    Function function;
    function.variables.push_back(variables[i]);
    function.table = vector<ldouble>(variables[i]->d, 1.0);
    function.cpt_var = variables[i];
    functions.push_back(function);
}

void GM::updateCPTs(Data &data, vector<ldouble> &weights) {
    for (auto &function : functions) {
        //Update Unary CPT
        if ((int) function.variables.size() == 1) {
            Utils::getXStat(function.variables[0], data, weights, function.table);
            Utils::normalize1d(function.table);
        } else if ((int) function.variables.size() == 2) {
            Variable *to = function.cpt_var;
            Variable *from;
            if (function.cpt_var->id == function.variables[0]->id) {
                from = function.variables[1];
            } else {
                if (function.cpt_var->id == function.variables[1]->id) {
                    from = function.variables[0];
                } else {
                    cerr << "Error in updating binary CPTs from data\n";
                    exit(-1);
                }
            }
            vector<vector<ldouble> > table;
            Utils::getXYStat(from, to, data, weights, table);
            for (int a = 0; a < from->d; a++) {
                from->t_value = a;
                ldouble norm_const = 0.0;
                for (int j = 0; j < to->d; j++) {
                    norm_const += table[a][j];
                }
                for (int j = 0; j < to->d; j++) {
                    to->t_value = j;
                    int addr = Utils::getAddress(function.variables);
                    function.table[addr] = table[a][j] / norm_const;
                }
            }
        }
    }
}

ldouble GM::getProbability(vector<int> &assignment) {
    for (int i = 0; i < assignment.size(); i++)
        variables[i]->t_value = assignment[i];
    ldouble p = 1.0;
    for (auto &function : functions) {
        p *= function.table[Utils::getAddress(function.variables)];
    }
    return p;
}

ldouble GM::getLogProbability(vector<int> &assignment) {
    for (int i = 0; i < assignment.size(); i++)
        variables[i]->t_value = assignment[i];
    ldouble p = 0.0;
    for (auto &function : functions) {
        p += log(function.table[Utils::getAddress(function.variables)]);
    }
    return p;
}

void GM::readUAI08(const string filename) {
    ifstream in(filename);
    string mystr;
    in >> mystr;
    if (mystr.find("BAYES") == string::npos) {
        cerr << "Something wrong with the file, cannot read\n";
        exit(-1);
    }
    int numvars;
    in >> numvars;
    variables = vector<Variable *>(numvars);
    for (int i = 0; i < numvars; i++) {
        int d;
        in >> d;
        variables[i] = new Variable(i, d);
    }
    int nfuncs;
    in >> nfuncs;
    functions = vector<Function>(nfuncs);
    for (int j = 0; j < nfuncs; j++) {
        int nvars;
        in >> nvars;
        functions[j].variables = vector<Variable *>(nvars);
        for (int k = 0; k < nvars; k++) {
            int varid;
            in >> varid;
            functions[j].variables[k] = variables[varid];
            if (k == nvars - 1) functions[j].cpt_var = variables[varid];
        }
    }
    for (int j = 0; j < nfuncs; j++) {
        vector<Variable*> func_variables=functions[j].variables;
        sort(func_variables.begin(),func_variables.end(),Utils::less_than_comparator_variable);
        vector<ldouble> &table = functions[j].table;
        int tab_size;
        in >> tab_size;
        table = vector<ldouble>(tab_size);
        for (int k = 0; k < tab_size; k++) {
            Utils::setAddress(functions[j].variables,k);
            int d=Utils::getAddress(func_variables);
            in >> table[d];
        }
        functions[j].variables=func_variables;
    }
    in.close();
}