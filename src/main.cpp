//
// Created by Vibhav Gogate on 9/22/19.
//

#include <Utils.h>
#include <GM.h>
#include <random>
#include <climits>
#include <Data.h>
#include <MT.h>
#include <myRandom.h>
#include <BTP.h>
#include <MT_BTP.h>

myRandom my_random;
void generate_random_tree_GM(GM& gm, int n)
{
    std::uniform_real_distribution<double> dist_real(0, 1);
    std::uniform_int_distribution<int> dist_int(0,INT_MAX-1);
    gm.variables=vector<Variable*>(n);
    for(int i=0;i<n;i++){
        gm.variables[i]=new Variable(i,2);
    }
    gm.functions=vector<Function>(n-1);
    //Generate a random permutation
    vector<int> inorder(n);
    vector<int> order(n);
    for(int i=0;i<n;i++)
        inorder[i]=i;

    for(int i=0;i<n;i++){
        int index = dist_int(Utils::rand_generator) % inorder.size();
        order[i] = inorder[index];
        swap(inorder[index], inorder[inorder.size() - 1]);
        inorder.pop_back();
    }
    cout<<"Printing order\n";
    for(int i=0;i<n;i++){
        cout<<order[i]<<" ";
    }
    cout<<endl;
    cout<<"-------------\n";
    for(int i=1;i<n;i++){
        gm.functions[i-1]=Function();
        if (order[i-1]<order[i]) {
            gm.functions[i - 1].variables.push_back(gm.variables[order[i - 1]]);
            gm.functions[i - 1].variables.push_back(gm.variables[order[i]]);
        }
        else{
            gm.functions[i - 1].variables.push_back(gm.variables[order[i]]);
            gm.functions[i - 1].variables.push_back(gm.variables[order[i-1]]);
        }
        gm.functions[i-1].table=vector<ldouble>(Utils::getDomainSize(gm.functions[i-1].variables));
        for(ldouble & j : gm.functions[i-1].table){
            j=dist_real(Utils::rand_generator);
        }
        Utils::normalizeAndGetNormConst(gm.functions[i-1]);
    }
    cout<<"GM generated\n";
}
void some_func()
{
    const int nrolls=100000;
    std::uniform_real_distribution<double> dist_real(0, 1);

    std::bernoulli_distribution distribution(0.5);

    int count=0;  // count number of trues

    for (int i=0; i<nrolls; ++i) if (distribution(Utils::rand_generator)) ++count;

    std::cout << "bernoulli_distribution (0.5) x 100000:" << std::endl;
    std::cout << "true:  " << count << std::endl;
    std::cout << "false: " << nrolls-count << std::endl;

    vector<vector<ldouble> >weights(5, vector<ldouble>(100,dist_real(Utils::rand_generator)));
    for(int i=0;i<weights.size();i++) {
        for (int j = 0; j < weights[i].size(); j++)
            cout << weights[i][j] << " ";
        cout << endl;
    }
    Utils::normalizeDim2(weights);
    for(int i=0;i<weights.size();i++) {
        for (int j = 0; j < weights[i].size(); j++)
            cout << weights[i][j] << " ";
        cout << endl;
    }
}
void old_funct()
{

    /*
    Variable var0=Variable(0,2);
    Variable var1=Variable(1,2);
    Variable var2=Variable(2,2);
    Variable var3=Variable(3,3);
    vector<Variable*> t1;
    vector<Variable*> t2;
    vector<Variable*> t3;
    t1.push_back(&var0);
    t1.push_back(&var1);
    t2.push_back(&var1);
    t2.push_back(&var2);
    t3.push_back(&var3);
    vector<Variable*> t;
    Utils::do_set_union(t1,t2,t);
    for(auto & i : t)
        cout<<i->id<<" ";
    cout<<endl;

    int address=5;
    cout<<address<<":";
    Utils::setAddress(t,address);
    for(auto & i : t)
        cout<<" "<<i->t_value;
    cout<<" : (new) "<<Utils::getAddress(t)<<endl;

    address=3;
    cout<<address<<":";
    Utils::setAddress(t,address);
    for(auto & i : t)
        cout<<" "<<i->t_value;
    cout<<" : (new) "<<Utils::getAddress(t)<<endl;

    address=7;
    cout<<address<<":";
    Utils::setAddress(t,address);
    for(auto & i : t)
        cout<<" "<<i->t_value;
    cout<<" : (new) "<<Utils::getAddress(t)<<endl;

    Utils::do_set_difference(t1,t2,t);
    for(auto & i : t)
        cout<<i->id<<" ";
    cout<<endl;

    Utils::do_set_difference(t2,t1,t);
    for(auto & i : t)
        cout<<i->id<<" ";
    cout<<endl;

    Utils::do_set_intersection(t1,t2,t);
    for(auto & i : t)
        cout<<i->id<<" ";
    cout<<endl;

    std::mt19937 g(100L);
    std::uniform_real_distribution<double> dist(0, 1);
    std::uniform_int_distribution<int> dist_int(0,INT_MAX-1);
    Function f1(t1);
    Function f2(t2);
    vector<Function*> functions;
    functions.push_back(&f1);
    functions.push_back(&f2);
    for(int i=0;i<f1.table.size();i++)
        f1.table[i]=dist(g);
    for(int i=0;i<f2.table.size();i++)
        f2.table[i]=dist(g);
    Function f;

    Utils::do_set_intersection(t1,t2,t);
    Utils::multiplyAndMarginalize(functions,t,f);
    f1.print();
    f2.print();
    f.print();

    cout<<"---------------------------\n";
    Utils::do_set_union(t1,t2,t);
    Utils::multiplyAndMarginalize(functions,t,f);
    f1.print();
    f2.print();
    f.print();

    cout<<"---------------------------\n";
    Utils::do_set_union(t1,t2,t);
    t.push_back(&var3);
    Utils::multiplyAndMarginalize(functions,t,f);
    f1.print();
    f2.print();
    f.print();
    */
    GM gm;
    generate_random_tree_GM(gm,100);

    GMSampler sampler;
    GM other_gm;
    vector<vector<ldouble> > var_marginals;
    for(int i=0;i<5;i++){
        gm.variables[rand()%gm.variables.size()]->value=rand()%2;
    }


    //for(auto & sampling_function : sampler.sampling_functions){
      //  sampling_function.print();
    //}
    //cout<<"Sanity check\n";
    //cout<<other_gm.getPosteriorGMSampler(sampler)<<endl;


    vector<vector<int> > assignments;
    sampler.generateSamples(100000,assignments);
    int j=rand()%100;
    vector<ldouble> counts(gm.variables[j]->d);
    for(int i=0;i<assignments.size();i++){
        counts[assignments[i][j]]++;
    }
    Utils::normalize1d(counts);
    Utils::printVector(counts,"Approximate:");
    Utils::printVector(var_marginals[j],"Exact:");


}
int main(int argc, char* argv[])
{
    //old_funct();
    //return 1;

    if (argc<2){
        cerr<<"Usage: "<<argv[0]<<" <dataset-filename>\n";
        exit(-1);
    }

    Data train_data,valid_data,test_data;
    //string dataset_name="/Users/vgogate//PycharmProjects/MarkovNetQuantization/data/nltcs";
    string dataset_name=argv[1];
    string tr_filename = dataset_name + ".ts.data";
    string valid_filename = dataset_name + ".valid.data";
    string test_filename = dataset_name + ".test.data";

    train_data.read(tr_filename);
    valid_data.read(valid_filename);
    test_data.read(test_filename);

    train_data.append(valid_data);
    cout<<"Data read\n";
    MT mt;
    mt.learn(train_data);
    cout<<"Test set likelihood = "<<Utils::getLLScore(test_data,mt)<<endl;


    /*
    for(int i=0;i<mt.ncomponents;i++){
        for(int j=0;j<mt.tree_gms[i].functions.size();j++)
            mt.tree_gms[i].functions[j].print();
    }*/
    srand(time(NULL));
    for(int i=0;i<10;i++){
        int var=rand()%mt.variables.size();
        int val=rand()%mt.variables[var]->d;
        mt.variables[var]->setValue(val);
    }

    MTSampler sampler;
    MT_BTP mtBtp(mt);
    vector<vector<ldouble> > var_marginals;

    ldouble pe=mtBtp.getPE();
    cout<<"PE = "<<pe<<endl;

    cout<<"Computing marginals\n";
    mtBtp.getVarMarginals(var_marginals);
    MT other_mt;
    cout<<"Computing Posterior Distribution\n";
    mtBtp.getPosteriorDist(other_mt);
    cout<<"Computing Posterior Sampler\n";
    mtBtp.getPosteriorSampler(sampler);
    cout<<"Computed Posterior Sampler\n";
    vector<vector<ldouble> > other_marginals;
    MT_BTP other_mt_btp(other_mt);
    other_mt_btp.getVarMarginals(other_marginals);

    cout<<"Computing Distance between two distributions \n";
    cout<<"Error between two = "<<Utils::computeAbsError(other_marginals,var_marginals)<<endl;
    vector<vector<int> > assignments;
    vector<ldouble> weights;
    sampler.generateSamples(1000*1010, assignments);
    for(int iter=1;iter<1010;iter+=100) {
        vector<vector<ldouble> > app_marginals(mt.variables.size());
        for(int i=0;i<mt.variables.size();i++){
            app_marginals[i]=vector<ldouble>(mt.variables[i]->d,0.0);
        }
        for(int i=0;i<iter*1000;i++){
            for(int j=0;j<mt.variables.size();j++){
                app_marginals[j][assignments[i][j]]+=1.0;//mt.getProbability(assignment);
            }
        }
        for(int i=0;i<mt.variables.size();i++){
            Utils::normalize1d(app_marginals[i]);
        }
        ldouble error=Utils::computeHellingerError(var_marginals,app_marginals);
        cout<<" N = "<<1000*iter<<" Err ="<<error<<endl;
    }

    // Test read write
    mt.write("tmpfile.mt");
    other_mt.read("tmpfile.mt");
    for(int i=0;i<mt.variables.size();i++){
        if(mt.variables[i]->isEvidence()){
            other_mt.variables[i]->value=mt.variables[i]->value;
        }
    }

    MT_BTP mtBtp1(mt),mtBtp2(other_mt);

    cout<<"PE due to mt "<<mtBtp1.getPE()<<endl;
    cout<<"PE due to other mt "<<mtBtp2.getPE()<<endl;


    GM gm_;
    gm_.readUAI08("tmpfile.uai");
    GMSampler gm_sampler_(gm_);
    vector<vector<int> > tr_assignments;
    gm_sampler_.generateSamples(10000,tr_assignments);
    gm_sampler_.generateSamples(10000,assignments);
    ldouble ll_score1=0.0;
    for(int i=0;i<assignments.size();i++){
        ll_score1+=log(gm_sampler_.getProbability(assignments[i]));
    }

    Data new_data(tr_assignments);
    mt.learn(new_data);
    ldouble ll_score=0.0;
    for(int i=0;i<assignments.size();i++){
        ll_score+=log(mt.getProbability(assignments[i]));
    }
    cout<<"Test data according to original model = "<<ll_score1/(ldouble)1000<<endl;
    cout<<"Test data according to learned model = "<<ll_score/(ldouble)1000<<endl;
}