//
// Created by Vibhav Gogate on 9/23/19.
//

//
//  MI.cpp
//  TPM-IS
//
//  Created by Vibhav Gogate on 3/23/19.
//  Copyright © 2019 Vibhav Gogate. All rights reserved.
//

#include <MI.h>
#include <MyTypes.h>
#include <Utils.h>

/*
    dsize: array having "nvars" entries storing the domain size of each variable
    data: matrix having nexamples x nvars
    error_check: when true, do a sanity check to see if dsize and data are compatible
 */
MI::MI(Data& data_, vector<ldouble>& weights)
{
    // Fetch number of variables and examples from data
    int nvars=data_.nfeatures;
    int nexamples=data_.nexamples;
    vector<int>& dsize=data_.dsize;
    vector<vector<int> > & data=data_.data_matrix;

    // Compute pxy and px

    // Initialize pxy
    pxy=vector<vector<vector<vector<ldouble > > > > (nvars);
    for(unsigned long i=0;i<nvars;i++){
        pxy[i]=vector<vector<vector<ldouble> > > (nvars);
        for(unsigned long j=0;j<nvars;j++){
            pxy[i][j]=vector<vector<ldouble> > (dsize[i]);
            for(unsigned long k=0;k<dsize[i];k++){
                pxy[i][j][k]=vector<ldouble> (dsize[j],1.0); //1-Laplace correction applied
            }
        }
    }
    // Initialize px
    px=vector<vector<ldouble> > (nvars);
    for(unsigned long i=0;i<nvars;i++){
        px[i]=vector<ldouble> (dsize[i],1.0);//1-Laplace correction applied
    }
    // Estimate pxy and px from data
    for(unsigned long i=0;i<nexamples;i++){
        for(unsigned long j=0;j<nvars;j++){
            px[j][data[i][j]]+=weights[i];
            for(unsigned long k=j+1;k<nvars;k++){
                pxy[j][k][data[i][j]][data[i][k]]+=weights[i];
            }
        }
    }

    //Normalize each pxy
    for(unsigned long i=0;i<nvars;i++){
        for(unsigned long j=i+1;j<nvars;j++){
            Utils::normalize2d(pxy[i][j]);
        }
    }
    //Normalize each px
    for(unsigned long i=0;i<nvars;i++)
        Utils::normalize1d(px[i]);

    //Convert px to log(px) for faster implementation
    vector<vector<ldouble> > lpx(nvars);
    for(unsigned long i=0;i<nvars;i++){
        lpx[i]=vector<ldouble>(dsize[i]);
        for(unsigned long j=0;j<dsize[i];j++){
            lpx[i][j]=log(px[i][j]);
        }
    }

    // Compute Mutual Information
    mi_matrix=vector<vector<ldouble> > (nvars);
    for(unsigned long i=0;i<nvars;i++){
        mi_matrix[i]=vector<ldouble> (nvars,0.0);
    }
    for(unsigned long i=0;i<nvars;i++){
        for(unsigned long j=i+1;j<nvars;j++){
            for(unsigned long a=0;a<dsize[i];a++) {
                for (unsigned long b = 0; b < dsize[j]; b++) {
                    ldouble log_val = log(pxy[i][j][a][b]) - lpx[i][a] - lpx[j][b];
                    mi_matrix[i][j] += pxy[i][j][a][b] * log_val;
                }
            }
        }
    }
}
