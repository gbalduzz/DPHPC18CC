import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os
import scipy.stats as st
import glob
import re

benchmark_dir1 = "10000000_nodes_500000000_edges/"

benchmark_dir2 = "500000_nodes_500000000_edges/"
benchmark_dir3 = "20000000_nodes_500000000_edges/"
fig_dir = ""

doComparison = False
doSpeedup = True

confidence_value = 0.95


plt.rcParams.update({'font.size': 16})

def confidenceInterval(data, prob_interval) :
	# Note: the probability of the confidence interval starting at k and -k -th data point
	#	   is  1 - 2 * binom(k, len(data), 0.5)

	p = np.ones(len(data)) / len(data)
	distro = st.rv_discrete(values = (data, p))
	point = distro.median()
	interval = distro.interval(prob_interval)
	return point, interval[0], interval[1]


def getOmpOnlySamples(filename, n):
    df = pd.read_csv(benchmark_dir + filename)
    data = df.values
    return [float(data[i,1]) for i in range(len(data[:,0])) if int(data[i,0]) == n]


def getOmpTimes(benchmark_dir, graph_size): 
	# omp only
	
	to_plot = []
	for t in range(1,33): 
		file_omp = "outputs/dynamic_schedule_omp_cray_" + str(graph_size) + "_vertices_" +str(t) +"_t.txt"
	
		data = pd.read_csv(file_omp,comment='#').values
		u = []
		l = []
		d,up,lw = confidenceInterval(data[0], 0.95)
		to_plot += [d]
		print("Loaded data: ", d, "For ",t)
	return to_plot



if(doSpeedup):
    ns = [i for i in range(1,33)]
    #data = getOmpTimes(benchmark_dir1,10000000)
    #speedups = [data[0] / x for x in data]
    
    data_2 = getOmpTimes(benchmark_dir2,500000)
    print(data_2)
    speedups_2 = [data_2[0] / x for x in data_2]

    
    #data_3 = getOmpTimes(benchmark_dir3, 20000000)
    #speedups_3 = [data_3[0] / x for x in data_3]
    plt.figure()
    plt.title("OMP only speedup")
    plt.xlim(0,24)
    plt.ylim(0,48)
    plt.xlabel("Number of cores")
    plt.ylabel("Speedup")

    #plt.plot(ns, speedups_3, marker='o',label="20'000'000 Vertices")
    #plt.plot(ns, speedups, marker='o', label="10'000'000 Vertices")
    plt.plot(ns, speedups_2, marker='o',label="500'000 Vertices")
    plt.plot(ns, ns, label="Linear")
    plt.legend(loc='upper left')
    plt.savefig(fig_dir + "omp_speedup_with_ref.eps",format='eps', dpi=40)







plt.show()



