import numpy as np
import matplotlib.pyplot as plt

path = []
inp = input()
while inp != "":
    path.append(tuple([float(xi) for xi in inp.replace("\n","").split(", ")]))
    inp = input()

path = np.array(path)
plt.plot(path[:,0], path[:,1])
plt.show()

