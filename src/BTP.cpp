//
// Created by Vibhav Gogate on 9/25/19.
//

#include <BTP.h>
#include <iostream>

BTP::BTP(GM &gm): pe(1.0),downward_performed(false),upward_performed(false) {
    //cout<<"Initing buckets\n";
    variables = gm.variables;
    Utils::getOrder(gm.variables, gm.functions, order);
    buckets = vector<vector<Function> >(order.size());
    var_in_pos = vector<int>(order.size());
    for (int i = 0; i < var_in_pos.size(); i++)
        var_in_pos[order[i]] = i;

    for (auto &function : gm.functions) {
        int pos = order.size();
        Function newf;
        function.instantiateEvidence(newf);
        for (auto &variable : newf.variables) {
            if (var_in_pos[variable->id] < pos)
                pos = var_in_pos[variable->id];
        }
        if (newf.variables.empty()){
            //function.print();
            pe*=newf.table[0];
            //cout<<pe<<"  "<<newf.table[0]<<endl;
            continue;
        }
        buckets[pos].push_back(newf);
    }
    //cout<<"Buckets initialized\n";
}


ldouble BTP::downwardPass() {
    if(downward_performed) return pe;
    for (int i = 0; i < buckets.size(); i++) {
        if (buckets[i].empty())
            continue;
        vector<Variable *> bucket_variables;
        for (auto & j : buckets[i]) {
            vector<Variable *> new_variables;
            Utils::do_set_union(bucket_variables, j.variables, new_variables);
            bucket_variables = new_variables;
        }
        vector<Variable *> bucket_variable;
        bucket_variable.push_back(variables[order[i]]);
        vector<Variable *> marg_variables;
        Utils::do_set_difference(bucket_variables, bucket_variable, marg_variables);
        Function function;
        Utils::multiplyAndMarginalize(buckets[i], marg_variables, function, false);

        if (function.variables.empty()) {
            pe *= function.table[0];
            continue;
        }
        pe *= Utils::normalizeAndGetNormConst(function);
        //Put the function in the appropriate bucket
        int pos = order.size();
        for (auto &variable : function.variables) {
            if (var_in_pos[variable->id] < pos)
                pos = var_in_pos[variable->id];
        }
        // Store info for reverse pass
        bt_edges.emplace_back(i, pos, buckets[pos].size());
        buckets[pos].push_back(function);
    }
    downward_performed=true;
    return pe;
}

void BTP::upwardPass() {
    if(upward_performed) return;
    //3. Reverse Pass
    //cout<<"Upward pass\n";
    for (int i = bt_edges.size() - 1; i > -1; i--) {

        int from = bt_edges[i].v2;
        int to = bt_edges[i].v1;
        int ignore_pos = bt_edges[i].v1_to_v2_message_pos;
        if (buckets[from].empty())
            continue;
        vector<Variable *> bucket_variables;
        vector<Function *> reorganized_bucket(buckets[from].size() - 1);
        //cout<<"Multiplying the following:\n";
        for (int j = 0, s = 0; j < buckets[from].size(); j++) {
            if (j == ignore_pos) continue;
            vector<Variable *> new_variables;
            Utils::do_set_union(bucket_variables, buckets[from][j].variables, new_variables);
            bucket_variables = new_variables;
            //buckets[from][j].print();
            reorganized_bucket[s++] = &buckets[from][j];
        }
        //cout<<"Skipped function\n";
        //buckets[from][ignore_pos].print();
        vector<Variable *> marg_variables;
        Utils::do_set_intersection(bucket_variables, buckets[from][ignore_pos].variables, marg_variables);
        Function function;
        Utils::multiplyAndMarginalize(reorganized_bucket, marg_variables, function, true);
        //cout<<"New function:\n";
        //function.print();
        if (!function.variables.empty()) {
            bt_edges[i].v2_to_v1_message_pos = buckets[to].size();
            buckets[to].push_back(function);
        }
    }
    upward_performed=true;
}
ldouble BTP::getPE()
{
    if(downward_performed){
        return pe;
    }
    downwardPass();
    return pe;
}
void BTP::propagate()
{
    if (downward_performed && upward_performed) return;
    if (!downward_performed) downwardPass();
    if (!upward_performed) upwardPass();
}

void BTP::getVarMarginals(vector<vector<ldouble> > &var_marginals) {
   propagate();
    var_marginals = vector<vector<ldouble> >(order.size());
    for (int i = 0; i < order.size(); i++) {
        int var = order[i];
        if (variables[var]->isEvidence()) {
            vector<ldouble> table(variables[var]->d, 0.0);
            table[variables[var]->value] = 1.0;
            var_marginals[var] = table;
            continue;
        }
        if (buckets[i].empty()) {
            vector<ldouble> table(variables[var]->d, (ldouble) 1.0 / (ldouble) variables[var]->d);
            var_marginals[var] = table;
            continue;
        }
        vector<Variable *> marg_variables;
        marg_variables.push_back(variables[var]);
        Function function;
        Utils::multiplyAndMarginalize(buckets[i], marg_variables, function);
        var_marginals[var] = function.table;
    }
}

void BTP::getPosteriorDist(GM &gm) {
    propagate();
    gm.variables = variables;
    gm.functions = vector<Function>();
    // Construct the GM
    for (int i = order.size() - 1; i > -1; i--) {
        int var = order[i];
        if (variables[var]->isEvidence()) continue;
        if (buckets[i].empty()) {
            //cout<<"Bucket is empty\n";
            Function function;
            function.variables.push_back(variables[var]);
            function.table = vector<ldouble>(variables[var]->d, (ldouble) 1.0 / (ldouble) variables[var]->d);
            gm.functions.push_back(function);
            //sampling_functions.emplace_back(function,variables[var]);
            continue;
        }
        vector<Variable *> bucket_variables;
        for (auto &j : buckets[i]) {
            vector<Variable *> new_variables;
            Utils::do_set_union(bucket_variables, j.variables, new_variables);
            bucket_variables = new_variables;
        }
        gm.functions.emplace_back();
        Function& function = gm.functions[gm.functions.size() - 1];
        Utils::multiplyAndMarginalize(buckets[i], bucket_variables, function, true);
        Utils::functionToCPT(function, variables[var]);

    }
}

void BTP::getPosteriorSampler(GMSampler &gm_sampler) {
    propagate();
    gm_sampler = GMSampler();
    gm_sampler.variables = variables;
    // Construct the GM
    for (int i = order.size() - 1; i > -1; i--) {
        int var = order[i];
        if (variables[var]->isEvidence()) {
            continue;
        }
        if (buckets[i].empty()) {
            cout<<"Bucket is empty\n";
            Function function;
            function.variables.push_back(variables[var]);
            function.table = vector<ldouble>(variables[var]->d, (ldouble) 1.0 / (ldouble) variables[var]->d);
            gm_sampler.sampling_functions.emplace_back(function, variables[var]);
            continue;
        }
        vector<Variable *> bucket_variables;
        for (auto &j : buckets[i]) {
            vector<Variable *> new_variables;
            Utils::do_set_union(bucket_variables, j.variables, new_variables);
            bucket_variables = new_variables;
        }
        Function function;
        Utils::multiplyAndMarginalize(buckets[i], bucket_variables, function, true);
        gm_sampler.sampling_functions.emplace_back(function, variables[var]);
        //gm_sampler.sampling_functions[gm_sampler.sampling_functions.size()-1].print();
    }
    gm_sampler.setEvidenceVariables();
}

