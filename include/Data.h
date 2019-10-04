//
// Created by Vibhav Gogate on 9/23/19.
//

#ifndef CNPROPOSAL_DATA_H
#define CNPROPOSAL_DATA_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include <MyTypes.h>

struct Data {

    int nexamples;
    int nfeatures;
    vector<vector<int> > data_matrix;
    vector<int> dsize;

    Data() : nexamples(0), nfeatures(0) {}

    explicit Data(vector<vector<int> > &data_matrix_);

    bool read(const string &dataset);

    void append(Data &data);
};

#endif //CNPROPOSAL_DATA_H
