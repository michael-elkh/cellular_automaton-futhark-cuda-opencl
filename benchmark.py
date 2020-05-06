import numpy as np
from os import sys, mkdir
from matplotlib import pyplot as plt
from matplotlib.pyplot import figure
import subprocess

ITERATIONS=10000
MAX_VALUE=16
ORDERS = [512, 1024, 2048, 4096, 8192, 16384]
BACKENDS=["cuda", "opencl"]

def panic(message:str):
    print(message)
    exit(1)

results = {}
for backend in BACKENDS:
    res = subprocess.run(["make", "TARGET="+backend, backend+"_bench", "futhark_{}_bench".format(backend)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if res.returncode != 0:
        print(res)
        panic("Something went wrong while creating the executables")

    results[backend] = {}
    for automaton in ["parity", "cyclic"]:
        results[backend][automaton] = {}
        for program in [backend+"_bench", "futhark_{}_bench".format(backend)]:
            name = "futhark" if "futhark" in program else "standard api" 
            results[backend][automaton][name] = []  
            for order in ORDERS:
                res = subprocess.run(["./"+program, str(ITERATIONS), str(order), str(order), automaton, str(MAX_VALUE)], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
                if res.returncode != 0:
                    print(res)
                    panic("Something went wrong while executing benchs")
                out = str(res.stdout, encoding="utf8").strip()
                float_res = float(out.split(" ")[1])
                Gcells_by_second = (ITERATIONS*order**2)/float_res/1.0e9
                results[backend][automaton][name].append(Gcells_by_second)

mkdir("plots")
for backend, measures in results.items():
    for automaton, version in measures.items():
        labels = list(map(str, ORDERS))
        x = np.arange(len(labels))  # the label locations
        width = 0.4  # the width of the bars

        fig, ax = plt.subplots(figsize=(7.5,5))
        rects1 = ax.bar(x - width/2, np.round(version['standard api'], 2), width, label=backend)
        rects2 = ax.bar(x + width/2, np.round(version['futhark'], 2), width, label="futhark "+backend)

        # Add some text for labels, title and custom x-axis tick labels, etc.
        ax.set_ylabel('GCells by second')
        ax.set_title("{0} {1} automaton".format(backend, automaton))
        ax.set_xticks(x)
        ax.set_xticklabels(labels)
        ax.set_xlabel('Square matrix cells by side')
        ax.margins(0.01, 0.25)
        ax.legend()

        for rects in [rects1, rects2]:
            #Attach a text label above each bar in *rects*, displaying its height.
            for rect in rects:
                height = rect.get_height()
                ax.annotate('{}'.format(height),
                            xy=(rect.get_x() + rect.get_width() / 2, height),
                            xytext=(0, 3),  # 3 points vertical offset
                            textcoords="offset points",
                            ha='center', va='bottom')

        plt.tight_layout()
        plt.savefig("plots/{0}_{1}.svg".format(backend, automaton))