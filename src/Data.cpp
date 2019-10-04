//
// Created by Vibhav Gogate on 9/23/19.
//

#include <Data.h>
#include <boost/tokenizer.hpp>

using namespace std;
Data::Data(vector<vector<int> > &data_matrix_) : data_matrix(data_matrix_) {
        nexamples = data_matrix.size();
        if (!data_matrix.empty()) {
            if (!data_matrix[0].empty()) nfeatures = data_matrix[0].size();
        }
        dsize = vector<int>(nfeatures, 1);
        for (int i = 0; i < nexamples; i++) {
            for (int j = 0; j < nfeatures; j++) {
                if (data_matrix[i][j] > (dsize[j] - 1)) dsize[j] = data_matrix[i][j] + 1;
            }
        }
}
bool Data::read(const string& dataset) {
    const string& filename = dataset;

    ifstream in(filename.c_str());
    if (!in.good()) {
        cerr << "Either the training, validation or test dataset file is missing\n";
        return false;
    }
    data_matrix.clear();
    vector<vector<int> > &examples = data_matrix;
    int i = 0;
    while (in.good()) {
        std::string s;
        std::getline(in, s);
        if (s.empty())
            break;
        examples.emplace_back();
        boost::char_separator<char> sep(",");
        boost::tokenizer<boost::char_separator<char> > tok(s, sep);
        for (boost::tokenizer<boost::char_separator<char> >::iterator iter =
                tok.begin(); iter != tok.end(); iter++) {
            examples[i].push_back(stoi(*iter));
        }
        i++;
    }
    nexamples = data_matrix.size();
    if (!data_matrix.empty()) {
        if (!data_matrix[0].empty()) nfeatures = data_matrix[0].size();
    }
    dsize = vector<int>(nfeatures, 1);
    for (int i = 0; i < nexamples; i++) {
        for (int j = 0; j < nfeatures; j++) {
            if (data_matrix[i][j] > (dsize[j] - 1)) dsize[j] = data_matrix[i][j] + 1;
        }
    }
    in.close();
    return !examples.empty();
}

void Data::append(Data &data) {
    // A small error check: if the number of features dont match, do not do anything
    if (nfeatures!=data.nfeatures && nexamples>0) return;
    // Append data to current data
    data_matrix.insert(data_matrix.end(),data.data_matrix.begin(),data.data_matrix.end());
    // Update the member variables: nexamples and nfeatures
    nexamples = data_matrix.size();
    if (!data_matrix.empty()) {
        if (!data_matrix[0].empty()) nfeatures = data_matrix[0].size();
    }
    // Update the domain sizes: dsize
    for (int i = 0; i < data.nexamples; i++) {
        for (int j = 0; j < data.nfeatures; j++) {
            if (data.data_matrix[i][j] > (dsize[j] - 1)) dsize[j] = data.data_matrix[i][j] + 1;
        }
    }
}

