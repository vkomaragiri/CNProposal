//
// Created by Vibhav Gogate on 9/22/19.
//

#include <random>
#include <climits>
#include <fstream>
#include <cstdlib>
#include <iomanip>
//#include <boost/program_options.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
//#include <boost/format.hpp>


#include <Utils.h>
#include <GM.h>
#include <Data.h>
#include <MT.h>
#include <myRandom.h>
#include <BTP.h>
#include <MT_BTP.h>

using namespace std;

boost::program_options::options_description desc(
        "MTProposal");

int parseOptions(int argc, char* argv[]) {
    string dataset,outfilename_mt,outfilename_marginals;
    try {

        //used (files .ts.data, .test.data, and .valid.data should be present in the directory)
        desc.add_options()
        ("help,?", "produce help message")
        ("interval,ivl", boost::program_options::value<int>(&HyperParameters::interval_for_structure_learning)->default_value(10), "Interval for structure learning in EM")
        ("num-iterations-em,nem", boost::program_options::value<int>(&HyperParameters::num_iterations_em)->default_value(100), "Max Number of iterations for EM")
        ("tolerance,tol",boost::program_options::value<ldouble>(&HyperParameters::tol)->default_value(1e-5),"Tolerance for EM LL scores")
        ("num-components,nc",boost::program_options::value<int>(&HyperParameters::num_components)->default_value(10),"Number of Mixture Components")
        ("number-of-samples,ns",boost::program_options::value<int>(&HyperParameters::num_samples)->default_value(10000),"Number of samples")
        ("dataset,d", boost::program_options::value<std::string>(&dataset), "Dataset")
        ("store-mt,omt", boost::program_options::value<std::string>(&outfilename_mt), "Store Mixture of Trees")
        ("store-var-marginals,ovm", boost::program_options::value<std::string>(&outfilename_marginals), "Store Variable Marginals")
        ;

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
        return 1;
    } catch (exception& e) {
        cerr << "error: " << e.what() << "\n";
        return 0;
    } catch (...) {
        cerr << "Exception of unknown type!\n";
        return 0;
    }
}
int main(){

}