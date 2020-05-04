import numpy as np
from os import sys
from matplotlib import pyplot
import subprocess

ITERATIONS=10000
MAX_VALUE=16

def panic(message:str):
    print(message)
    exit(1)

res = subprocess.run(["make", "cuda_bench", "opencl_bench"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
if res.returncode != 0:
    print(res)
    panic("Something went wrong while creating the executables")

orders = [256, 512, 1024, 2048]

results = {}
for program in ["cuda_bench", "opencl_bench"]:
    results[program] = {}
    for automaton in ["parity", "cyclic"]:
        results[program][automaton] = []
        for i in orders:
            res = subprocess.run(["./"+program, str(ITERATIONS), str(i), str(i), automaton, str(MAX_VALUE)], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
            if res.returncode != 0:
                print(res)
                panic("Something went wrong while executing benchs")
            out = str(res.stdout, encoding="utf8").strip()
            results[program][automaton].append(float(out.split(" ")[1]))
        
print(results)