import numpy as np
from os import sys, makedirs
from matplotlib import pyplot as plt
from matplotlib.pyplot import figure
import subprocess

ITERATIONS=1
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

    for automaton in ["parity", "cyclic"]:
        if automaton not in results:
            results[automaton] = {}
        for program in [backend+"_bench", "futhark_{}_bench".format(backend)]:
            name = program.replace("_bench", "").replace("_", " ")
            results[automaton][name] = []
            for order in ORDERS:
                res = subprocess.run(["./"+program, str(ITERATIONS), str(order), str(order), automaton, str(MAX_VALUE)], stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
                if res.returncode != 0:
                    print(res)
                    panic("Something went wrong while executing benchs")
                out = str(res.stdout, encoding="utf8").strip()
                float_res = float(out.split(" ")[1])
                Gcells_by_second = (ITERATIONS*order**2)/float_res/1.0e9
                results[automaton][name].append(np.round(Gcells_by_second, 2))

makedirs("plots", exist_ok=True)
for automaton, measures in results.items():
    labels = list(map(str, ORDERS))
    x = np.arange(len(labels))  # the label locations
    width = 0.2  # the width of the bars

    fig, ax = plt.subplots(figsize=(13,6.5))
    rects = []
    pos = x - 1.5*width
    for version, values in measures.items():
        rects.append(ax.bar(pos, np.round(values, 2), width, label=version))
        pos += width
    
    # Add some text for labels, title and custom x-axis tick labels, etc.
    ax.set_ylabel('GCells by second')
    ax.set_title("{0} automaton".format(automaton))
    ax.set_xticks(x)
    ax.set_xticklabels(labels)
    ax.set_xlabel('Square matrix cells by side')
    ax.margins(0.01, 0.3)
    ax.legend()

    for rect in rects:
        #Attach a text label above each bar in *rects*, displaying its height.
        for r in rect:
            height = r.get_height()
            ax.annotate('{}'.format(height),
                        xy=(r.get_x() + r.get_width() / 2, height),
                        xytext=(0, 3),  # 3 points vertical offset
                        textcoords="offset points",
                        ha='center', va='bottom')

    plt.tight_layout()
    plt.savefig("plots/{0}.svg".format(automaton))
