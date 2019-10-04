//
// Created by Vibhav Gogate on 9/27/19.
//

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <Data.h>
#include <Variable.h>
#include <HyperParameters.h>
#include <MT.h>
#include <MTSampler.h>
#include <MT_BTP.h>
// Include the headers of MyLib

using namespace boost::python;

namespace p = boost::python;
namespace np = boost::python::numpy;
typedef vector<int> vec;
typedef vector<vec> mat;

class MT_PY_Interface {
    MT mt;
    MT_BTP mtBtp;
    MTSampler mtSampler;
    vector<vector<int> > assignments;
public:
    MT_PY_Interface()= default;
    static np::ndarray convert_to_numpy(mat const &input) {
        u_int n_rows = input.size();
        u_int n_cols = input[0].size();
        p::tuple shape = p::make_tuple(n_rows, n_cols);
        np::dtype dtype = np::dtype::get_builtin<int>();
        np::ndarray converted = np::zeros(shape, dtype);

        for (u_int i = 0; i < n_rows; i++) {
            for (u_int j = 0; j < n_cols; j++) {
                converted[i][j] = input[i][j];
            }
        }
        return converted;
    }
    void learn(const string& tr_filename, const string& valid_filename)
    {
        Data train_data,valid_data;
        train_data.read(tr_filename);
        valid_data.read(valid_filename);
        train_data.append(valid_data);
        mt.learn(train_data);
    }
    void read(const string& mt_filename){
        mt.read(mt_filename);
    }
    void write(const string& mt_filename){
        mt.write(mt_filename);
    }
    void setEvidence(int var, int val)
    {
        mt.variables[var]->setValue(val);
    }
    np::ndarray getPosteriorSamples(int n)
    {
        assignments.clear();
        mtBtp=MT_BTP(mt);
        cout << "PE = " << mtBtp.getPE() << endl;
        mtBtp.getPosteriorSampler(mtSampler);
        mtSampler.generateSamples(n, assignments);
        return convert_to_numpy(assignments);
    }
    np::ndarray getPriorSamples(int n)
    {
        assignments.clear();
        mtSampler=MTSampler(mt);
        mtSampler.generateSamples(n,assignments);
        return convert_to_numpy(assignments);
    }
    np::ndarray getWeights() {
        vector<ldouble> v_(assignments.size());
        vector<double> v(assignments.size());
        for(int i=0;i<v.size();i++){
           v_[i] = mtSampler.getProbability(assignments[i]);
           cout<<v_[i]<<" ";
           v[i]= static_cast<double> (log(v_[i]));
           cout<<v[i]<<endl;
        }
        cout<<endl;
        int v_size = v.size();
        p::tuple shape = p::make_tuple(v_size);
        p::tuple stride = p::make_tuple(sizeof(double));
        np::dtype dt = np::dtype::get_builtin<double>();
        np::ndarray output = np::from_data(&v[0], dt, shape, stride, p::object());
        np::ndarray output_array = output.copy();
        return output_array;
    }
};

class BN_PY_Interface{
    GM gm;
    GMSampler gmSampler;
    BTP btp;
    vector<vector<int> > assignments;
public:
    BN_PY_Interface()= default;
    static np::ndarray convert_to_numpy(mat const &input) {
        u_int n_rows = input.size();
        u_int n_cols = input[0].size();
        p::tuple shape = p::make_tuple(n_rows, n_cols);
        np::dtype dtype = np::dtype::get_builtin<int>();
        np::ndarray converted = np::zeros(shape, dtype);

        for (u_int i = 0; i < n_rows; i++) {
            for (u_int j = 0; j < n_cols; j++) {
                converted[i][j] = input[i][j];
            }
        }
        return converted;
    }
    void setEvidence(int var, int val)
    {
        gm.variables[var]->setValue(val);
    }
    void read(string filename){
        gm.readUAI08(filename);
    }
    np::ndarray getPriorSamples(int n)
    {
        assignments.clear();
        gmSampler=GMSampler(gm);
        gmSampler.generateSamples(n,assignments);
        return convert_to_numpy(assignments);
    }
};

BOOST_PYTHON_MODULE(GMLib)
{
    np::initialize();
    class_<HyperParameters>("HyperParameters")
            .def("setNumIterationsEM",&HyperParameters::setNumIterationsEM).staticmethod("setNumIterationsEM")
            .def("setIntervalForStructureLearning",&HyperParameters::setIntervalForStructureLearning).staticmethod("setIntervalForStructureLearning")
            .def("setNumComponents",&HyperParameters::setNumComponents).staticmethod("setNumComponents")
            .def("setTolerance",&HyperParameters::setTolerance).staticmethod("setTolerance")
            ;
    class_<MT_PY_Interface>("MT")
            .def("read",&MT_PY_Interface::read)
            .def("write",&MT_PY_Interface::write)
            .def("learn",&MT_PY_Interface::learn)
            .def("setEvidence",&MT_PY_Interface::setEvidence)
            .def("getPosteriorSamples",&MT_PY_Interface::getPosteriorSamples)
            .def("getPriorSamples",&MT_PY_Interface::getPriorSamples)
            .def("getWeights",&MT_PY_Interface::getWeights)
            ;
    class_<BN_PY_Interface>("BN")
            .def("read",&BN_PY_Interface::read)
            .def("setEvidence",&BN_PY_Interface::setEvidence)
            .def("getPriorSamples",&BN_PY_Interface::getPriorSamples)
            ;
    class_<Data>("Data")
            .def("read", &Data::read)
            .def_readonly("nexamples", &Data::nexamples)
            .def_readonly("nfeatures",&Data::nfeatures)
            .def("append", &Data::append)
            ;
    class_<Variable>("Variable")
            .def_readwrite("id",&Variable::id)
            .def_readwrite("value",&Variable::value)
            .def_readwrite("d",&Variable::d)
            .def("setValue",&Variable::setValue)
            .def("getValue",&Variable::getValue)
            .def("isEvidence",&Variable::isEvidence)
            ;
}
