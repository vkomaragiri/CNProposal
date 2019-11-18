//
// Created by Vibhav Gogate on 10/10/19.
//

#include <iomanip>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <cmath>

#include <Utils.h>
#include <Data.h>
#include <MT.h>
#include <MTSampler.h>
#include <myRandom.h>
#include <HyperParameters.h>
#include <MT_BTP.h>

using namespace std;

boost::program_options::options_description desc(
        "MTProposal");
string mtfilename, outfilename, evidfilename, fname;


std::istream &operator>>(std::istream &in, ORDERING_HEURISTIC &ordering_heu) {
    std::string token;
    in >> token;
    if (token == "0")
        ordering_heu = min_fill;
    else if (token == "1")
        ordering_heu = min_degree;
    else if (token == "2")
        ordering_heu = topological;
    else
        in.setstate(std::ios_base::failbit);
    return in;
}

int parseOptions(int argc, char *argv[]) {

    try {

        //used (files .ts.data, .test.data, and .valid.data should be present in the directory)
        desc.add_options()
                ("help,?", "produce help message")
                ("mtfile,m", boost::program_options::value<std::string>(&mtfilename), "MT model file")
                ("outfile,o", boost::program_options::value<std::string>(&outfilename), "Store MAR")
                ("heuristic,h", boost::program_options::value<ORDERING_HEURISTIC>(&HyperParameters::ord_heu),
                 "Ordering Heuristic")
                ("evidfile,e", boost::program_options::value<std::string>(&evidfilename), "Evidence file")
                ("fname,f", boost::program_options::value<std::string>(&fname), "File Name");


        boost::program_options::variables_map vm;
        boost::program_options::store(
                boost::program_options::parse_command_line(argc, argv, desc),
                vm);
        boost::program_options::notify(vm);

        if(!fname.empty()){
            mtfilename = mtfilename+fname;
            outfilename = outfilename+fname+".MAR";
            evidfilename = evidfilename+fname+".evid";
        }
        else{
            cout << "No file name provided\n";
            cout << desc << endl;
            vm.clear();
            exit(-1);
        }
        if (vm.count("help")) {
            cout << desc << endl;
            vm.clear();
            return 0;
        }
        if (mtfilename.empty()) {
            cout << " No probabilistic model files provided\n";
            cout << desc << endl;
            vm.clear();
            exit(-1);
        }
        return 1;
    } catch (exception &e) {
        cerr << "error: " << e.what() << "\n";
        return 0;
    } catch (...) {
        cerr << "Exception of unknown type!\n";
        return 0;
    }
}

int main(int argc, char *argv[]) {
    if (parseOptions(argc, argv) == 0) {
        return 0;
    }
    //Importance Sampling
    MT mt;
    mt.read(mtfilename);
    if (!evidfilename.empty()) {
        ifstream in(evidfilename);

        int num_evidences;
        in >> num_evidences;
        for (int i = 0; i < num_evidences; i++) {
            int var, val;
            in >> var;
            in >> val;
            mt.variables[var]->setValue(val);
        }
        in.close();
    }

    MT_BTP mt_btp = MT_BTP(mt);
    vector<vector<ldouble>> mar;
    mt_btp.getVarMarginals(mar);
    vector<vector<int>> samples;
    MTSampler mts = MTSampler(mt);
    mts.generateSamples(100000, samples);

    vector<vector<ldouble>> post_mar = vector<vector<ldouble> >(mt.variables.size());
    for(int i=0;i<post_mar.size();i++){
        post_mar[mt.variables[i]->id]=vector<ldouble>(mt.variables[i]->d,0.0);
    }

    for(int n = 0; n < samples.size(); n++){
        for(auto var: mt.variables){
            post_mar[var->id][samples[n][var->id]]++;
        }
    }
    for(int i = 0; i < post_mar.size(); i++){
        Utils::normalize1d(post_mar[i]);
    }
    for(int i = 0; i < mt.variables.size(); i++){
        cout << mt.variables[i]->id << " ";
        Utils::printVector(mar[i]);
        Utils::printVector(post_mar[i]);
        cout << endl;
    }
}
