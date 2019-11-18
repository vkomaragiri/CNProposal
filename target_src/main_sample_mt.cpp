//
// Created by Vibhav Gogate on 10/10/19.
//

#include <iomanip>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


#include <Utils.h>
#include <Data.h>
#include <MT.h>
#include <MT_BTP.h>
#include <myRandom.h>
#include <HyperParameters.h>

using namespace std;

boost::program_options::options_description desc(
        "MTProposal");
string infilename, outfilename = "tmpfile.mar", evidfilename, fname;


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
                ("pmfile,i", boost::program_options::value<std::string>(&infilename), "Probabilistic model file")
                ("outfile,o", boost::program_options::value<std::string>(&outfilename), "Store Variable Marginals")
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
            infilename = infilename+fname;
            outfilename = outfilename+fname;
            evidfilename = evidfilename+fname;
        }

        if (vm.count("help")) {
            cout << desc << endl;
            vm.clear();
            return 0;
        }
        if (infilename.empty()) {
            cout << " No probabilistic model file provided\n";
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
    MT mt;
    mt.read(infilename);
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
    MT_BTP mt_btp(mt);
    vector<vector<ldouble> > var_marginals;
    mt_btp.getVarMarginals(var_marginals);
    Utils::printMarginals(var_marginals, outfilename);
    cout << "P(E=e): " << mt_btp.getPE() << endl;
}
