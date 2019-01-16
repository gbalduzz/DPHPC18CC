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


plt.rcParams.update({'font.size': 12})

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


def getOmpTimesDaint(graph_size): 
	# omp only
	
	to_plot = []
	for t in range(1,25): 
		file_omp = "outputs/omp_cray_" + str(graph_size) + "_vertices_" +str(t) +"_t.txt"
	
		data = pd.read_csv(file_omp,comment='#').values
		d,up,lw = confidenceInterval(data[0], 0.95)
		to_plot += [d]
		print("Loaded data: ", d, "For ",t)
	return to_plot

def getOmpTimes(benchmark_dir): 
	# omp only
	file_omp = benchmark_dir + "omp/omp_results.log"
	data = pd.read_csv(file_omp,sep=',').values
	u = []
	l = []
	to_plot = []
	#ignore first
	for i in range(0, 24): 
		d,up,lw = confidenceInterval(data[i*20+1:i*20+21,2], 0.95)
		to_plot += [d]
		print(d)
	return to_plot




if(doSpeedup):
    ns = [i for i in range(1,25)]
    data = getOmpTimesDaint(10000000)
    speedups = [data[0] / x for x in data]
    
    data_2 = getOmpTimesDaint(500000)
    print(data_2)
    speedups_2 = [data_2[0] / x for x in data_2]

    
    data_3 = getOmpTimesDaint(20000000)
    speedups_3 = [data_3[0] / x for x in data_3]
    plt.figure()
 

    plt.plot(ns, speedups_3, marker='s',markersize=8,label="20'000'000 Vertices, Daint",color='firebrick')
    plt.plot(ns, speedups, marker='o',markersize=8, label="10'000'000 Vertices, Daint",color='lightsalmon')
    plt.plot(ns, speedups_2, marker='^',markersize=8,label="500'000 Vertices, Daint", color='saddlebrown')



    ns = [i for i in range(1,25)]
    data = getOmpTimes(benchmark_dir1)
    speedups = [data[0] / x for x in data]
    
    data_2 = getOmpTimes(benchmark_dir2)
    speedups_2 = [data_2[0] / x for x in data_2]

    
    data_3 = getOmpTimes(benchmark_dir3)
    speedups_3 = [data_3[0] / x for x in data_3]

    plt.plot(ns, speedups_3, marker='s',markersize=8,label="20'000'000 Vertices, Euler",color='navy')
    plt.plot(ns, speedups, marker='o',markersize=8, label="10'000'000 Vertices, Euler",color='lightblue')
    plt.plot(ns, speedups_2, marker='^',markersize=8,label="500'000 Vertices, Euler",color='mediumslateblue')

    plt.title("OMP only speed up")
    plt.xlim(0,25)
    plt.ylim(0,25)
    plt.xlabel("Number of cores")
    plt.ylabel("Speed up")
    plt.plot(ns, ns, label="Linear",color='green')
    plt.tight_layout(pad=0.1,w_pad=0.1, h_pad=0.1)
    plt.legend(loc='upper left')
    plt.savefig(fig_dir + "omp_speedup_with_ref.pdf",format='pdf')







plt.show()



