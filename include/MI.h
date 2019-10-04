//
// Created by Vibhav Gogate on 9/23/19.
//

#ifndef CNPROPOSAL_MI_H
#define CNPROPOSAL_MI_H
//
//  MI.h
//  TPM-IS
//
//  Created by Vibhav Gogate on 3/19/19.
//  Copyright © 2019 Vibhav Gogate. All rights reserved.
//

#ifndef MI_h
#define MI_h
#include <iostream>
#include <vector>
#include <cstdlib>

#include <MyTypes.h>
#include <Data.h>

using namespace std;
struct MI{
    vector<vector<ldouble> > mi_matrix;
    vector<vector<vector<vector<ldouble > > > > pxy;
    vector<vector<ldouble> > px;
    MI(){}
    MI(Data& data, vector<ldouble>& weights);
};

#endif /* MI_h */

#endif //CNPROPOSAL_MI_H
