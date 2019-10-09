//
// Created by Vibhav Gogate on 9/23/19.
//

#include <MT.h>
#include <MI.h>
#include <random>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <HyperParameters.h>

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, ldouble> > Graph;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef std::pair<int, int> E;

struct SparseTree {
    vector<vector<int> > edges;
    vector<bool> visited;
    int visited_size;
    vector<E> directed_edges;
    vector<int> roots;

    SparseTree() :
            visited_size(0) {
    }

    explicit SparseTree(int n) :
            edges(vector<vector<int> >(n)), visited_size(0) {
    }

    void addEdge(int i, int j) {
        edges[i].push_back(j);
        edges[j].push_back(i);
    }

    void ResetVisited() {
        visited = vector<bool>(edges.size());
        for (int i = 0; i < (int) edges.size(); i++)
            visited[i] = false;
        visited_size = 0;
    }

    void DFS_recurse(int i) {
        visited[i] = true;
        visited_size++;
        for (int j = 0; j < (int) edges[i].size(); j++) {
            int v = edges[i][j];
            if (!visited[v]) {
                directed_edges.emplace_back(i, v);
                DFS_recurse(v);
            }
        }
    }

    void DFS() {
        ResetVisited();
        directed_edges.clear();
        roots.clear();
        int i = 0;
        while (visited_size != (int) edges.size()) {
            if (!visited[i]) {
                DFS_recurse(i);
                roots.push_back(i);
            }
            ++i;
        }
    }
};

void MT::learn(Data &data) {
    std::uniform_real_distribution<double> dist_real(0, 1);
    ncomponents = HyperParameters::num_components;
    this->probabilities = vector<ldouble>(ncomponents);
    this->variables = vector<Variable *>(data.nfeatures);
    this->tree_gms = vector<GM>(ncomponents);
    for (int i = 0; i < variables.size(); i++)
        variables[i] = new Variable(i, data.dsize[i]);
    for (int i = 0; i < ncomponents; i++) {
        tree_gms[i].variables = variables;
    }
    // Initialize the weight vector randomly
    vector<vector<ldouble> > weights(ncomponents, vector<ldouble>(data.nexamples));
    for (int i = 0; i < ncomponents; i++) {
        for (int j = 0; j < data.nexamples; j++) {
            weights[i][j] = dist_real(myRandom::m_g);
        }
    }
    Utils::normalizeDim2(weights);
    for (int j = 0; j < 10; j++) {
        for (int i = 0; i < ncomponents; i++) {
            cout << weights[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Starting EM\n";
    for (int iter = 1; iter <= HyperParameters::num_iterations_em; iter++) {
        // E-step: Compute the Chow-Liu trees at each component
        for (int c = 0; c < ncomponents; c++) {
            probabilities[c] = Utils::sum1d(weights[c]);
            if (iter == 1 || iter % HyperParameters::interval_for_structure_learning == 0) {
                //if(1){
                MI mi(data, weights[c]);
                Graph g(mi.mi_matrix.size());
                for (int i = 0; i < (int) mi.mi_matrix.size(); i++) {
                    for (int j = i + 1; j < (int) mi.mi_matrix.size(); j++) {
                        if (mi.mi_matrix[i][j] > HyperParameters::mi_low_limit) {
                            boost::add_edge(i, j, -mi.mi_matrix[i][j], g);
                        }
                    }
                }
                std::vector<Edge> spanning_tree;
                boost::kruskal_minimum_spanning_tree(g, std::back_inserter(spanning_tree));
                SparseTree MST(data.nfeatures);
                for (auto &ei : spanning_tree) {
                    int v1 = boost::source(ei, g);
                    int v2 = boost::target(ei, g);
                    MST.addEdge(v1, v2);

                }
                MST.DFS();
                this->tree_gms[c].functions.clear();
                for (auto &directed_edge : MST.directed_edges) {
                    int from = directed_edge.first;
                    int to = directed_edge.second;
                    this->tree_gms[c].addBinaryCPT(from, to);
                }
                for (int root : MST.roots) {
                    this->tree_gms[c].addUnaryCPT(root);
                }
            }
            this->tree_gms[c].updateCPTs(data, weights[c]);
        }
        Utils::normalize1d(probabilities);

        // M-Step
        long double iter_ll = 0.0;
        for (int j = 0; j < data.nexamples; j++) {
            long double p = 0.0;
            for (int i = 0; i < ncomponents; i++) {
                weights[i][j] = this->probabilities[i]
                                * this->tree_gms[i].getProbability(data.data_matrix[j]);
                p += weights[i][j];
            }
            iter_ll += log(p);
        }
        cout << "LL at iter " << iter << " = " << iter_ll / (ldouble) data.nexamples << endl;
        Utils::normalizeDim2(weights);
    }
}

ldouble MT::getProbability(vector<int> &example) {
    ldouble p = 0.0;
    for (int j = 0; j < this->ncomponents; j++) {
        p += this->tree_gms[j].getProbability(example) * this->probabilities[j];
    }
    return p;
}

ldouble MT::getLogProbability(vector<int> &example) {
   return log(getProbability(example));
}
void MT::write(const string &filename) {
    ofstream out(filename);
    out << "MT\n";
    out << this->ncomponents << " " << this->variables.size() << "\n";
    for (auto &variable : variables) {
        out << variable->d << " ";
    }
    out << endl;
    for(int i=0;i<ncomponents;i++){
        out<<probabilities[i]<<" ";
    }
    out<<endl;
    for (int i = 0; i < this->tree_gms.size(); i++) {
        out << tree_gms[i].functions.size() << "\n";
        for (auto &function : this->tree_gms[i].functions) {
            out << function.variables.size() << " ";
            for (auto &variable : function.variables) {
                out << variable->id << " ";
            }
            out << endl;
        }
        for (auto &function : this->tree_gms[i].functions) {
            out << function.table.size() << "\n";
            for (long double k : function.table) {
                out << k << " ";
            }
            out << endl;
        }
    }
    out.close();
}

void MT::read(const string &filename)
{
    ifstream in(filename);
    string mystr;
    in>>mystr;
    if(mystr.find("MT")==string::npos){
        cerr<<"Something wrong with the file, cannot read\n";
        exit(-1);
    }
    int numvars;
    in>>ncomponents;
    in>>numvars;

    variables=vector<Variable*>(numvars);
    for(int i=0;i<numvars;i++) {
        int d;
        in >> d;
        variables[i] = new Variable(i, d);
    }

    probabilities=vector<ldouble>(ncomponents);
    for(int i=0;i<ncomponents;i++){
        ldouble tmp;
        in>>tmp;
        probabilities[i]=tmp;
    }
    tree_gms=vector<GM>(ncomponents);
    for(int i=0;i<ncomponents;i++){
        int nfuncs;
        in>>nfuncs;
        tree_gms[i].variables=variables;
        tree_gms[i].functions=vector<Function>(nfuncs);
        for (int j=0;j<nfuncs;j++) {
            int nvars;
            in>>nvars;
            tree_gms[i].functions[j].variables=vector<Variable*> (nvars);
            for (int k=0;k<nvars;k++) {
                int varid;
                in>>varid;
                tree_gms[i].functions[j].variables[k]=variables[varid];
            }
        }

        for (int j=0;j<nfuncs;j++) {
            vector<ldouble>& table=tree_gms[i].functions[j].table;
            int tab_size; in>>tab_size;
            table=vector<ldouble>(tab_size);
            for (int k=0;k<tab_size;k++) {
                in>>table[k];
            }
        }
    }
    in.close();
}
/*
ldouble MT::getPosteriorMT(MT &mt) {
    mt = MT();
    mt.ncomponents = this->ncomponents;
    mt.variables = variables;
    mt.probabilities = vector<ldouble>(this->ncomponents);
    mt.tree_gms = vector<GM>(this->ncomponents);

    ldouble pe = 0.0;
    for (int i = 0; i < this->ncomponents; i++) {
        mt.probabilities[i] = this->probabilities[i] * this->tree_gms[i].getPosteriorGM(mt.tree_gms[i]);
        pe += mt.probabilities[i];
    }
    Utils::normalize1d(mt.probabilities);
    return pe;
}

ldouble MT::getPosteriorMTSampler(MTSampler &mtSampler) {
    vector<ldouble> post_probabilities(this->ncomponents);
    mtSampler.variables = variables;
    mtSampler.samplers = vector<GMSampler>(this->ncomponents);
    ldouble pe = 0.0;
    for (int i = 0; i < ncomponents; i++) {
        ldouble current_pe=tree_gms[i].getPosteriorGMSampler(mtSampler.samplers[i]);
        //cout<<"PE at component "<<i<< " is "<<current_pe<<endl;
        post_probabilities[i] = probabilities[i] * current_pe;
        pe += post_probabilities[i];
    }
    Utils::normalize1d(post_probabilities);
    mtSampler.mixture_distribution = discrete_distribution<int>(post_probabilities.begin(), post_probabilities.end());
    return pe;
}

ldouble MT::getPosteriorMarginals(vector<vector<ldouble> > &var_marginals){

    vector<ldouble> post_probabilities(this->ncomponents);
    var_marginals=vector<vector<ldouble> >(variables.size());
    for(int i=0;i<var_marginals.size();i++){
        var_marginals[i]=vector<ldouble>(variables[i]->d,0.0);
    }
    ldouble pe = 0.0;
    for (int i = 0; i < ncomponents; i++) {
        vector<vector<ldouble > > tmp_marginals;
        post_probabilities[i] = probabilities[i] * tree_gms[i].getPosteriorMarginals(tmp_marginals);
        for(int j=0;j<variables.size();j++){
            for(int k=0;k<variables[j]->d;k++)
                var_marginals[j][k]+=tmp_marginals[j][k];
        }
        pe += post_probabilities[i];
    }
    for(auto & var_marginal : var_marginals){
        Utils::normalize1d(var_marginal);
    }
    return pe;
}
 */