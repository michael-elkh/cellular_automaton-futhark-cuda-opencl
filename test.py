from ac import ac
import numpy as np
import matplotlib.pyplot as plt
fut_lib = ac()

A = np.array([
        [False, False, True,  False], 
        [True,  True,  True,  True], 
        [True,  False, False, False], 
        [True,  False, True,  False]
    ], dtype=np.bool)

plt.plot(A)
plt.show()
print(fut_lib.ac_parity(A))