import matplotlib
import matplotlib.pyplot as plt
matplotlib.rcParams['figure.figsize'] = (10, 6)

import numpy as np
from scipy.optimize import curve_fit

def n(n, a, b):
    return a + b*n

def nlogn(n, a, b):
    return a + b*n*np.log(n)

def nsqrtn(n, a, b, c):
    return a + b*n + c*n**1.5

def nsq(n, a, b):
    return a + b*n**2

with open("metingen.csv", "r") as f:
    lines = f.readlines()

results = [[int(col) for col in line.split(", ")] for line in lines[1:]]
x   = np.array([res[0] for res in results]) / 1e3
bw  = np.array([res[1] for res in results]) / 1e6
hil = np.array([res[2] for res in results]) / 1e6
sor = np.array([res[3] for res in results]) / 1e6

# Fit the curves: popt_* contains the coefficients.
popt_bw,  pcov_bw  = curve_fit(nsqrtn, x, bw)
popt_hil, pcov_hil = curve_fit(n, x, hil)
popt_sor, pcov_sor = curve_fit(nsqrtn, x, sor)

# Define the curves.
curve_bw  = lambda x: nsqrtn(x, *popt_bw)
curve_hil = lambda x: n(x, *popt_hil)
curve_sor = lambda x: nsqrtn(x, *popt_sor)

plt.figure(figsize=(7,5))

plt.plot(x, bw, "r+", label="Bowyer-Watson")
# plt.plot(x, curve_bw(x), "k")

plt.plot(x, sor, "g+", label="Sorted x")
# plt.plot(x, curve_sor(x), "k")

plt.plot(x, hil, "b+", label="Hilbert")
# plt.plot(x, curve_hil(x), "k")

plt.xlabel("size (1000 pts)")
plt.ylabel("time (s)")
plt.legend()
plt.show()
