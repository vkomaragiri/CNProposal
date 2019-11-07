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
        "Generate evidence in inverse topological order from BN");
string infilename, evidfilename;
float ev;


int parseOptions(int argc, char *argv[]) {

    try {

        //used (files .ts.data, .test.data, and .valid.data should be present in the directory)
        desc.add_options()
                ("help,?", "produce help message")
                ("pmfile,i", boost::program_options::value<std::string>(&infilename), "BN model file")
                ("evidfile,o", boost::program_options::value<std::string>(&evidfilename), "Output Evidence file")
                ("evid_percent,e", boost::program_options::value<std::float_t>(
                        reinterpret_cast<float *>(&ev)), "Number of samples");



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
    vector<int> order;
    Utils::getTopologicalOrder(gm.variables, gm.functions, order);

    myRandom rgen = myRandom();
    if (!evidfilename.empty()) {
        ofstream out(evidfilename);
        int j = order.size()-1;
        out << (int) (ev/100*gm.variables.size()) << " ";
        for(int i = 0; i < (int)(ev/100*gm.variables.size()); i++)
        {
            int var = order[j];
            int val = rgen.getInt(gm.variables[var]->d);
            out << var << " " << val << " ";
            j--;
        }
        out << endl;
    }
}


