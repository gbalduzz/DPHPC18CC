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


benchmark_dir = benchmark_dir1
confidence_value = 0.95

# Returns point estimate, begin and end of confidence interval.
def confidenceInterval(data, prob_interval) :
    # Note: the probability of the confidence interval starting at k and -k -th data point
    #       is  1 - 2 * binom(k, len(data), 0.5)

    p = np.ones(len(data)) / len(data)
    distro = st.rv_discrete(values = (data, p))
    point = distro.median()
    interval = distro.interval(prob_interval)
    return point, interval[0], interval[1]


def plot_mean_and_CI(x, mean, lb, ub, color_mean, color_shading, linewidth=1, label=None):
    # plot the shaded range of the confidence intervals
    plt.fill_between(x, ub, lb,
                     color=color_shading, alpha=.5)
    # plot the mean on top
    plt.plot(x, mean, color_mean, linewidth=linewidth, label=label,marker='o')

def getComAvoidingSamples(filename):
    file = open(benchmark_dir + "cc_avoiding/"+ filename, 'r')
    samples = [float(x.split(",")[5]) for x in file.readlines() if x.startswith("/cluster")]
    file.close()
    return samples


def getSamples(col_name, filename, n):
    try:
        if(col_name == "Total compute"):
            values_scatter = np.array(getSamples("Scatter", filename, n))
            values_compute = np.array(getSamples("Compute", filename, n))
            values_reduce = np.array(getSamples("Reduce", filename, n))
            return values_scatter + values_compute + values_reduce

        df = pd.read_csv(filename)
        data = df.values
       
        column_names = np.array([x.replace(" ", "") for x in df.columns.values])
        column_names = np.delete(column_names, np.argwhere(column_names == 'Countnodes'))
        col_id = np.where(column_names == col_name.replace(" ", ""))[0][0]
        values = np.max((data[:,col_id]).reshape((-1,n)), axis=1)
        #print(values)
        return values
    except:
        print("failed for " + filename)

def getMixedFilename(n, t):
    n_mpi = int(n/t)
    return "n_" + str(n_mpi) + "_t_" + str(t) + ".txt"

def getMpiOnlyFilename(n):
    return "mpi/mpi_" + str(n) + ".log"

# setup plots



for benchmark_dir in [benchmark_dir1, benchmark_dir2, benchmark_dir3]:
	plt.rcParams.update({'font.size': 24})
	fig = plt.figure(figsize=(10, 10))
	linewidth = 1
	titles = ["Total time"]

	# plot mpi only
	plt.xlim(0,24)
	ns = [1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,41,43,47]
	for i, name in enumerate(["Total compute"]):
	    data = np.array([confidenceInterval(getSamples(name, benchmark_dir + getMpiOnlyFilename(n), n), confidence_value) for n in ns])
	    plot_mean_and_CI(ns, data[:,1], data[:,1], data[:,2], color_mean='#144ead', color_shading="#5790ed", linewidth=linewidth, label="mpi only")
	    plt.title(titles[i])
	    plt.xlabel("Number of cores")
	    plt.ylabel("Time [s]")

	#GET OMP
	file_omp = benchmark_dir + "omp/omp_results.log"
	data = pd.read_csv(file_omp,sep=',').values
	u = []
	l = []
	to_plot = []
	print(data[1:20,1])
	#ignore first
	for i in range(0, 24): 
	    d,up,lw = confidenceInterval(data[i*20+1:i*20+21,2], 0.95)
	    to_plot += [d]
	    u += [up]
	    l += [lw]
	plot_mean_and_CI([i for i in range(1,25)], to_plot, u, l,color_mean="red", linewidth=linewidth, color_shading="#ff9372", label="OMP")

	# plot comm avoiding
	ns = [i for i in range(1,25)]
	data = np.array([confidenceInterval(getComAvoidingSamples('cc_'+ str(n) +'.txt'), confidence_value) for n in ns])
	plot_mean_and_CI(ns, data[:,0], data[:,1], data[:,2], color_mean='green', color_shading="#87D37C", linewidth=linewidth, label="comm avoiding")


	plt.xlabel("Number of cores")
	plt.ylabel("Time [s]")
	plt.ylim(bottom=0, top=45)
	plt.legend()
	fig.tight_layout()

	plt.savefig(benchmark_dir[0:5] + "our_impl.eps", format='eps', dpi=40)
	plt.figure()

