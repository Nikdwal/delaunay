import matplotlib.pyplot as plt
import numpy as np

with open("splits.csv", "r") as f:
  lines = f.readlines()

lines = lines[1:]
lines = [line.replace("\n", "") for line in lines]
ns = [int(lines[i]) for i in range(len(lines)) if i % 4 == 0]
bw = [tuple([float(entry) for entry in lines[i].split(", ")]) for i  in range(len(lines)) if i % 4 == 1]
hil = [tuple([float(entry) for entry in lines[i].split(", ")]) for i  in range(len(lines)) if i % 4 == 2] 
sor = [tuple([float(entry) for entry in lines[i].split(", ")]) for i  in range(len(lines)) if i % 4 == 3] 

hil = np.array(hil)
sor = np.array(sor)
bw = np.array(bw)
ns = np.array(ns)

plt.plot(ns, bw[:,1])
plt.plot(ns, hil[:,1])
plt.plot(ns, sor[:,1])
plt.show()

plt.plot(ns, bw[:,2])
plt.plot(ns, hil[:,2])
plt.plot(ns, sor[:,2])
plt.show()

