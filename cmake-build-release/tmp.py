import GMLib
import numpy as np
d=GMLib.Data()
d.read('/Users/vgogate/PycharmProjects/MarkovNetQuantization/data/nltcs.ts.data')
print(d.nexamples)
print(d.nfeatures)
v=GMLib.Variable()
v.id=10
v.d=5
print(v.id)
print(v.d)
v.setValue(2)
print(v.getValue())
print(v.isEvidence())
# getSamples(string mt_filename, HyperParameters hyp_parameters, int n){
mt=GMLib.MT()
tr_file='/Users/vgogate/PycharmProjects/MarkovNetQuantization/data/plants.ts.data'
valid_file='/Users/vgogate/PycharmProjects/MarkovNetQuantization/data/plants.valid.data'

GMLib.HyperParameters.setNumIterationsEM(1000)
mt.learn(tr_file,valid_file)
mt.setEvidence(10,0)
mt.setEvidence(11,1)
samples=mt.getSamples(10)
weights=mt.getWeights()
print(samples)
print(weights)
