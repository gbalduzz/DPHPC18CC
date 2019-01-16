import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
import scipy.stats as st
import glob
import re

benchmark_dir1 = "outputs/"

benchmark_dir2 = "outputs/"
benchmark_dir3 = "outputs/"
fig_dir = ""

doComparison = False
doSpeedup = True

confidence_value = 0.95

plt.rcParams.update({'font.size': 16})
# Returns point estimate, begin and end of confidence interval.
def confidenceInterval(data, prob_interval) :
    # Note: the probability of the confidence interval starting at k and -k -th data point
    #       is  1 - 2 * binom(k, len(data), 0.5)

    p = np.ones(len(data)) / len(data)
    distro = st.rv_discrete(values = (data, p))
    point = distro.median()
    interval = distro.interval(prob_interval)
    return point, interval[0], interval[1]

def getSamples(col_name, filename, n):
    df = pd.read_csv(filename,delimiter = '\t',comment='#')
    data = df.values
    print("************************")
    print(data[:,0])
        
    return data[:,0]




def getMpiOnlyFilename(n, graph):
    return "ours_cray_" +str(graph) + "_vertices_" + str(n) + "_procs.txt"


if(doSpeedup):
    ns = [1,2,4,8,12,16,18,20,24,28,32,36,40,44,48,52,56,60,64]
    data = np.array([confidenceInterval(getSamples("Total compute", benchmark_dir1 + getMpiOnlyFilename(n, 10000000), n), confidence_value) for n in ns])[:,0]
    speedups = [data[0] / x for x in data]
    
    data_2 = np.array([confidenceInterval(getSamples("Total compute", benchmark_dir2 + getMpiOnlyFilename(n,500000), n), confidence_value) for n in ns])[:,0]
    speedups_2 = [data_2[0] / x for x in data_2]

    
    data_3 = np.array([confidenceInterval(getSamples("Total compute", benchmark_dir3 + getMpiOnlyFilename(n,20000000), n), confidence_value) for n in ns])[:,0]
    speedups_3 = [data_3[0] / x for x in data_3]
    plt.figure()
    plt.title("Mpi only speed up")
    plt.xlim(0,47)
    plt.xlabel("Number of cores")
    plt.ylabel("Speedup")

    plt.plot(ns, speedups_3, marker='o',label="20'000'000 Vertices")
    plt.plot(ns, speedups, marker='o', label="10'000'000 Vertices")
    plt.plot(ns, speedups_2, marker='o',label="500'000 Vertices")
    plt.plot(ns, ns, label="Linear")
    plt.legend()
    plt.savefig(fig_dir + "mpi_speedup_with_ref.eps", dpi=40, format='eps')







plt.show()



