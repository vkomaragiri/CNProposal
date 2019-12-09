import numpy as np
import scipy, copy, os, json
#import pandas as pd
from sklearn.linear_model import LogisticRegressionCV, LogisticRegression
from sklearn.preprocessing import normalize
import time, sys
from scipy.special import softmax, expit

#import pyximport; pyximport.install()

def read_data(fpath):
    data = np.array([[int(x) for x in line.split(',')[:-1]] for line in open(fpath, "r").readlines()])
    #return pd.read_csv(fpath, dtype=np.int32, header=None).to_numpy(dtype=np.int32)
    #print(data.shape, data[0])
    return data

def learn_logistic_classifier(X, y):
    C=[10**i for i in range(-7, 0)]
    clf = LogisticRegressionCV(max_iter=1000, tol=1e-7, cv = 3, penalty='l2', solver='saga', random_state=0, Cs=C, n_jobs=-1, multi_class='multinomial').fit(X, y)
    #clf = LogisticRegression(penalty='elasticnet', solver='saga', random_state=0, C = c, l1_ratio = r, njobs=-1).fit(X, y)
    return clf.coef_[0], clf.intercept_, clf

def fetch_flat_ind(arr, domains):
    ind = 0
    prod = 1
    for i in range(len(arr)-1, -1, -1):
        ind += arr[i]*prod
        prod *= domains[i]
    return ind

def learn_cpt(X, domains):
    cpt = 1/(X.shape[0])*np.ones(np.prod(domains))
    for i in range(X.shape[0]):
        ind = fetch_flat_ind(X[i, :], domains)
        cpt[ind] += 1
    mar = [np.sum(cpt[i:i+domains[-1]]) for i in range(0,cpt.shape[0], domains[-1])]
    cpt = [cpt[i]/mar[i/domains[-1]] for i in range(cpt.shape[0])]
    #mar = [cpt[2*i]+cpt[2*i+1]+1 for i in range(int(cpt.shape[0]/2))]
    #cpt = [cpt[i]/mar[int(i/2)] for i in range(cpt.shape[0])]
    return cpt

def learn_params(train_data, valid_data, test_data):
    order = list(range(train_data.shape[1]))
    params = []
    np.random.shuffle(order)
    domains = np.array([len(np.unique(train_data[:, i])) for i in range(train_data.shape[1])])
    for i in range(len(order)):
        if(i > 5):
            x_par = order[:i]
            y_col = order[i]
            X = train_data[:, x_par]
            y = train_data[:, y_col]
            flag = -1
            for d in range(domains[i]):
                if np.sum(y==d)/y.shape[0] > 0.99:
                    probs = 1.0/(len(domains)-1)*np.ones(len(domains))
                    probs[d] = 0.99
                    params.append(["B", order[:i+1], probs])
                    flag = 1
                    break
            if flag > 0:
                continue
            w, b, clf = learn_logistic_classifier(X, y)
            params.append(["L", order[:i+1], np.hstack([w, b]), clf])
        else:
            x_col = order[:i+1]
            X = train_data[:, x_col]
            cpt = learn_cpt(X, domains[x_col])
            params.append(["C", x_col, cpt])
        print(params[-1])
    #print(log_likelihood(train_data, params),log_likelihood(test_data, params))
    return params, domains, order

def log_likelihood(X, params):
    res = 0
    res2 = 0
    for i in range(X.shape[0]):
        for j in range(len(params)):
            if params[j][0] == "C":
                ind = fetch_flat_ind(X[i, params[j][1]])
                res += np.log(params[j][2][ind])
                res2 += np.log(params[j][2][ind])
            elif params[j][0] == "B":
                res += np.log(params[j][2][X[i, params[j][1][-1]]])
                res2 += np.log(params[j][2][X[i, params[j][1][-1]]])
            else:
                prob = params[j][3].predict_proba(X[i, params[j][1][:-1]].reshape((1, -1)))[0][X[i, params[j][1][-1]]]
                decision = np.dot(X[i, params[j][1][:-1]], params[j][2][:-1])+params[j][2][-1]
                p = expit(decision)
                prob2 =  np.array([1 - p, p])[X[i, params[j][1][-1]]]
                res += np.log(prob)
                res2 += np.log(prob2)
    res /= X.shape[0]
    res2 /= X.shape[0]
    return res

def write_params_to_file(params, f, domains, order):
    f.write("BAYES\n")
    f.write(str(len(order))+"\n")
    f.write(str(order)+"\n")
    f.write(str(domains)+"\n")
    for i in range(len(params)):
        if params[i][0] == "B":
            f.write("B\n")
        elif params[i][0] == "C":
            f.write("C\n")
        else:
            f.write("L\n")
        for var in params[i][1]:
            f.write(str(var)+" ")
        f.write("\n")
        for prob in params[i][2]:
            f.write(str(prob)+" ")
        f.write("\n")


def main():
    start_time = time.time()
    data_dir = sys.argv[2]
    fcbn_dir = sys.argv[4]
    ll_dir = sys.argv[6]
    inpfile = sys.argv[8]

    print(data_dir+inpfile)
    train_data = read_data(data_dir+inpfile+".ts.data")
    #train_val_data = read_data(data_dir+inpfile+".tv.data")
    valid_data = read_data(data_dir+inpfile+".valid.data")
    test_data = read_data(data_dir+inpfile+".test.data")

    train_data = np.vstack([train_data, valid_data])

    params, domains, order = learn_params(train_data, valid_data, test_data)
    #print(params)
    print("Time taken:", time.time()-start_time)
    ll = log_likelihood(test_data, params)
    print("Test Log-Likelihood:", ll, '\n')
    f = open(ll_dir+inpfile+".ll", "w")
    f.write(str(ll)+'\n')
    f.close()

    f = open(fcbn_dir+inpfile+".FCBN", "w")
    write_params_to_file(params, f, domains, order)
    f.close()

if __name__ == '__main__':
    main()