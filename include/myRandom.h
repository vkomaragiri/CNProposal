//
// Created by Vibhav Gogate on 9/26/19.
//

#ifndef CNPROPOSAL_MYRANDOM_H
#define CNPROPOSAL_MYRANDOM_H

#include <MyTypes.h>
#include <random>
#include <chrono>
using namespace std;
struct myRandom {
    static unsigned long int seed;
    static mt19937 m_g;
    static uniform_int_distribution<int> int_dist;
    static uniform_real_distribution<double> double_dist;
    // choose one of the random number generators:

    myRandom() {
        seed=std::chrono::system_clock::now().time_since_epoch().count();
        m_g.seed(seed);
    }
    static void setSeed(unsigned long int seed_)
    {
        seed=seed_;
        m_g.seed(seed);
    }
    static double getDouble() {
        return double_dist(m_g);
    }
    static int getInt()
    {
        return int_dist(m_g);
    }
    static int getInt(int max_value) {
        return int_dist(m_g)%max_value;
    }
};


#endif //CNPROPOSAL_MYRANDOM_H
