import numpy as np
from os import sys
from matplotlib import pyplot as plt
import subprocess

ITERATIONS=10000
MAX_VALUE=16
ORDERS = [512, 1024, 2048, 4096, 8192]
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

#results = {'cuda': {'parity': {'standard api': [4.95546313799622, 6.6873469387755105, 7.602508609751677, 7.805534567786359, 7.928085342658334], 'futhark': [4.511944922547332, 6.436930632289748, 7.4711506946918425, 7.771907166350118, 7.934647008051834]}, 'cyclic': {'standard api': [4.9182739212007505, 6.8669024230517355, 7.594249502082202, 7.601819664703218, 7.084748582709584], 'futhark': [4.543223570190642, 6.286426858513189, 7.527465900933238, 7.720762080073631, 7.870161135217544]}}, 'opencl': {'parity': {'standard api': [5.686420824295011, 6.529115815691158, 7.281777777777777, 7.831769209224162, 7.668007038552069], 'futhark': [3.4267189542483663, 5.330838840874428, 7.105376926986278, 7.69173665871997, 7.82619785653477]}, 'cyclic': {'standard api': [5.761406593406593, 6.607284183994959, 7.522065997130559, 7.696323684572687, 7.841744353170754], 'futhark': [3.3781443298969074, 5.39946446961895, 7.0789940928270045, 7.614931009440813, 7.863797794677697]}}}
for backend, measures in results.items():
    for automaton, version in measures.items():
        labels = list(map(str, ORDERS))
        x = np.arange(len(labels))  # the label locations
        width = 0.35  # the width of the bars

        fig, ax = plt.subplots()
        rects1 = ax.bar(x - width/2, np.round(version['standard api'], 2), width, label=backend)
        rects2 = ax.bar(x + width/2, np.round(version['futhark'], 2), width, label="futhark "+backend)

        # Add some text for labels, title and custom x-axis tick labels, etc.
        ax.set_ylabel('GCells by second')
        ax.set_title("{0} {1} automaton".format(backend, automaton))
        ax.set_xticks(x)
        ax.set_xticklabels(labels)
        ax.set_xlabel('Square matrix cells by side')
        ax.margins(0.05, 0.25)
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

        plt.savefig("{0}_{1}.svg".format(backend, automaton))