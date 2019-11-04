//
// Created by vasundhara on 10/28/19.
//

#include <iomanip>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


#include <Utils.h>
#include <GM.h>
#include <myRandom.h>
#include <GMSampler.h>

using namespace std;

boost::program_options::options_description desc(
        "Sample from BN");
string infilename, outfilename, evidfilename;
unsigned long int num_samples = 10;

int parseOptions(int argc, char *argv[]) {

    try {

        //used (files .ts.data, .test.data, and .valid.data should be present in the directory)
        desc.add_options()
                ("help,?", "produce help message")
                ("pmfile,i", boost::program_options::value<std::string>(&infilename), "Probabilistic model file")
                ("outfile,o", boost::program_options::value<std::string>(&outfilename), "Store samples")
                ("evidfile,e", boost::program_options::value<std::string>(&evidfilename), "Evidence file")
                ("num_samples,n", boost::program_options::value<std::uint64_t>(
                        reinterpret_cast<unsigned long *>(&num_samples)), "Number of samples");


        boost::program_options::variables_map vm;
        boost::program_options::store(
                boost::program_options::parse_command_line(argc, argv, desc),
                vm);
        boost::program_options::notify(vm);

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
    GM gm;
    gm.readUAI08(infilename);
    if (!evidfilename.empty()) {
        ifstream in(evidfilename);

        int num_evidences;
        in >> num_evidences;
        for (int i = 0; i < num_evidences; i++) {
            int var, val;
            in >> var;
            in >> val;
            gm.variables[var]->setValue(val);
        }

        in.close();
    }
    GMSampler gms(gm);
    gms.setEvidenceVariables();
    vector<vector<int> > samples;
    gms.generateSamples(num_samples, samples);
    ofstream os(outfilename);
    for(auto s : samples)
    {
        for(auto i : s)
        {
            os << i << ",";
        }
        os << endl;
    }
}


