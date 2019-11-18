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

using namespace std;

boost::program_options::options_description desc(
        "MTProposal");
string bnfilename, mtfilename, outfilename_is, outfilename_lw, evidfilename, fname;


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
                ("bnfile,b", boost::program_options::value<std::string>(&bnfilename), "BN model file")
                ("mtfile,m", boost::program_options::value<std::string>(&mtfilename), "MT model file")
                ("outfile_ImpS,is", boost::program_options::value<std::string>(&outfilename_is), "Store PE from Importance Sampling")
                ("outfile_LW,lw", boost::program_options::value<std::string>(&outfilename_lw), "Store PE from Likelihood Weighting")
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
            bnfilename = bnfilename+fname;
            mtfilename = mtfilename+fname;
            outfilename_lw = outfilename_lw+fname+".PR";
            outfilename_is = outfilename_is+fname+".PR";
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
        if (bnfilename.empty() || mtfilename.empty()) {
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
    ofstream is_out(outfilename_is), lw_out(outfilename_lw);

    GM bn;
    bn.readUAI08(bnfilename);

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

    vector<vector<int>> samples;
    MTSampler mts = MTSampler(mt);
    mts.generateSamples(100000, samples);

    long double imp_wt = 0;
    int k = 0, p = 2, z;
    for(auto s: samples)
    {
        imp_wt += bn.getProbability(s)/mts.getProbability(s);
        z = pow(10, p);

        k++;
        if(k%z == 0) {
            is_out << k << " " << imp_wt/k << endl;
            p++;
        }

    }
    //Likelihood Weighting
    //GM bn_copy = GM(bn);
    if (!evidfilename.empty()) {
        ifstream in(evidfilename);

        int num_evidences;
        in >> num_evidences;
        for (int i = 0; i < num_evidences; i++) {
            int var, val;
            in >> var;
            in >> val;
            bn.variables[var]->setValue(val);
        }

        in.close();
    }
    vector<vector<int>> lwsamples;
    GMSampler bns = GMSampler(bn);

    bns.generateSamples(100000, lwsamples);
    imp_wt = 0;
    p = 2, k = 0;
    for(auto s: lwsamples)
    {
        imp_wt += bn.getProbability(s)/bns.getProbability(s);

        k++;
        int z = pow(10, p);
        if(k%z == 0) {
            lw_out << k << " " << imp_wt/k << endl;
            p++;
        }

    }
}
