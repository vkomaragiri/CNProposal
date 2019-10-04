//
// Created by Vibhav Gogate on 9/26/19.
//

#ifndef CNPROPOSAL_MYRANDOM_H
#define CNPROPOSAL_MYRANDOM_H



#include <cstdlib>
using namespace std;
struct myRandom {
    unsigned long int seed;
    // choose one of the random number generators:

    myRandom() {

        seed=1000L;
        srand(seed);
    }
    void setSeed(int seed_)
    {
        //seed=time(NULL);
        seed=seed_;
        srand(seed);
    }
    double getDouble() {
        return (double)rand()/((double)2147483648UL);
    }
    int getInt()
    {
        return rand();
    }
    int getInt(int max_value) {
        return rand()%max_value;
    }
};

#endif //CNPROPOSAL_MYRANDOM_H
