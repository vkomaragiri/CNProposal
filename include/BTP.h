//
// Created by Vibhav Gogate on 9/25/19.
//

#ifndef CNPROPOSAL_BTP_H
#define CNPROPOSAL_BTP_H

#include <vector>
#include <MyTypes.h>
#include <Variable.h>
#include <Function.h>
#include <GM.h>
#include <SamplingFunction.h>
#include <GMSampler.h>
#include <InferenceEngine.h>

using namespace std;

struct BT_Edges {
    // Downward messages go from v1 to v2
    // Upward messages go from v2 to v1
    int v1;
    int v2;
    // position of message from v1 to v2 in bucket of v2
    int v1_to_v2_message_pos;
    // position of message from v2 to v1 in bucket of v1
    int v2_to_v1_message_pos;

    BT_Edges() : v1(-1), v2(-1), v1_to_v2_message_pos(-1), v2_to_v1_message_pos(-1) {}

    BT_Edges(int i1, int i2, int i3) : v1(i1), v2(i2), v1_to_v2_message_pos(i3), v2_to_v1_message_pos(-1) {}

    BT_Edges(int i1, int i2, int i3, int i4) : v1(i1), v2(i2), v1_to_v2_message_pos(i3), v2_to_v1_message_pos(i4) {}
};

// Class Bucket Tree Propagation
class BTP : public MarginalInferenceEngine, PosteriorDistCreator<GM>, PosteriorSamplerCreator<GMSampler> {
    vector<Variable *> variables;
    vector<vector<Function> > buckets;
    vector<int> order;
    vector<int> var_in_pos;
    vector<BT_Edges> bt_edges;
    ldouble pe;
    bool downward_performed;
    bool upward_performed;

    ldouble downwardPass();

    void upwardPass();

public:
    BTP():pe(1.0),downward_performed(false),upward_performed(false){}
    explicit BTP(GM &gm);

    ~BTP() {
        variables.clear();
        buckets.clear();
        order.clear();
        var_in_pos.clear();
    }

    void propagate();

    ldouble getPE() override;

    void getVarMarginals(vector<vector<ldouble> > &var_marginals) override;

    void getPosteriorDist(GM &gm) override;

    void getPosteriorSampler(GMSampler &gm_sampler) override;
};

#endif //CNPROPOSAL_BTP_H
