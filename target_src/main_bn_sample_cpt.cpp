//
// Created by Vibhav Gogate on 9/22/19.
//


#include <iomanip>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>


#include <Utils.h>
#include <Data.h>
#include <MT.h>
#include <myRandom.h>

using namespace std;

boost::program_options::options_description desc(
        "MTProposal");
string dataset, infile, outfile, fname;
Data train_data, valid_data, test_data;


int parseOptions(int argc, char *argv[]) {

    try {

        //used (files .ts.data, .test.data, and .valid.data should be present in the directory)
        desc.add_options()
                ("help,?", "produce help message")
                ("dataset,d", boost::program_options::value<std::string>(&dataset), "Dataset (without extensions)")
                ("infile,i", boost::program_options::value<std::string>(&infile), "Input BN file")
                ("outfile,o", boost::program_options::value<std::string>(&outfile), "Output BN file")
                ("fname,f", boost::program_options::value<std::string>(&fname), "File Name");

        boost::program_options::variables_map vm;
        boost::program_options::store(
                boost::program_options::parse_command_line(argc, argv, desc),
                vm);
        boost::program_options::notify(vm);
        if(!fname.empty()){
            infile = infile+fname;
            outfile = outfile+fname;
            dataset = dataset+fname;
        }

        if (vm.count("help")) {
            cout << desc << endl;
            vm.clear();
            return 0;
        }
        if (infile.empty()) {
            cout << " No probabilistic model file provided\n";
            cout << desc << endl;
            vm.clear();
            exit(-1);
        }
        if(outfile.empty()){
            cout << " No storage model file provided\n";
            cout << desc << endl;
            vm.clear();
            exit(-1);
        }
        if (!dataset.empty()) {
            bool ret_value;
            ret_value = train_data.read(dataset + ".ts.data");
            if (!ret_value)
                return 0;
            ret_value = valid_data.read(dataset + ".valid.data");
            if (!ret_value)
                return 0;
            ret_value = test_data.read(dataset + ".test.data");
            if (!ret_value)
                return 0;
            return 1;
        } else {
            cout << desc << endl;
            return 0;
        }
        return 0;
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
    train_data.append(valid_data);

    GM gm;
    gm.readUAI08(infile);
    gm.updateParams(train_data);
    gm.write(outfile);
}