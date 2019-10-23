import GMLib

d=GMLib.Data()
d.read('/home/vasundhara/UTD/Research/Mixture_of_Trees/datasets/nltcs.ts.data')
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