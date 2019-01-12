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

doComparison = True
doSpeedup = False
doMixed = False
confidence_value = 0.95

# Returns point estimate, begin and end of confidence interval.
def confidenceInterval(data, prob_interval) :
	# Note: the probability of the confidence interval starting at k and -k -th data point
	#	   is  1 - 2 * binom(k, len(data), 0.5)

	p = np.ones(len(data)) / len(data)
	distro = st.rv_discrete(values = (data, p))
	point = distro.median()
	interval = distro.interval(prob_interval)
	return point, interval[0], interval[1]



def plot_mean_and_CI(x, mean, lb, ub, color_mean, color_shading, linewidth=1, markerSize=1,  label=None):
	# plot the shaded range of the confidence intervals
	plt.fill_between(x, ub, lb, alpha=.5)
	# plot the mean on top
	plt.plot(x, mean, linewidth=linewidth, marker='o', markersize=markerSize, label=label)


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

		return values
	except:
		print("failed for " + filename)

def getComAvoidingSamples(filename):
	file = open(filename, 'r')
	samples = [float(x.split(",")[5]) for x in file.readlines() if x.startswith("/cluster")]
	file.close()
	return samples

def getOmpOnlySamples(filename, n):
	df = pd.read_csv(benchmark_dir + filename)
	data = df.values
	return [float(data[i,1]) for i in range(len(data[:,0])) if int(data[i,0]) == n]



def getMpiOnlyFilename(n):
	return benchmark_dir + "mpi/mpi_" + str(n) + ".log"

def getMixedFilename(n, t):
	n_mpi = int(n/t)
	return benchmark_dir + "mixed/mixed_nodes_" + str(n_mpi) + "_therads_" + str(t) + ".log"

def getCommAvoidingFilename(n):
	return benchmark_dir + "cc_avoiding/cc_" + str(n) + ".txt"





if(doComparison):
	# setup plots
	plt.rcParams.update({'font.size': 50})
	fig, axarr = plt.subplots(1, 4, figsize=(80, 30))
	linewidth = 2
	markerSize = 20
	titles = ["Scatter time", "Compute time", "Reduction time", "Total time"]




	# plot mpi only
	ns = [1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,41,43,47]
	for i, name in enumerate(["Scatter", "Compute", "Reduce", "Total compute"]):
		data = np.array([confidenceInterval(getSamples(name, getMpiOnlyFilename(n), n), confidence_value) for n in ns])
		plt.sca(axarr[i])
		plot_mean_and_CI(ns, data[:,0], data[:,1], data[:,2], color_mean='#144ead', color_shading="#5790ed", linewidth=linewidth, markerSize=markerSize, label="mpi only")





	# plot mixed
	if(doMixed): 
		t = 2
		#For 50
		ns = [x*t for x in [1,3,5,6,7,9,11,12,13,14,15,16,17,18,23,24]]
		#For 20
		#ns = [x*t for x in [1,2,6,11,12,13,14,15,16,17,18,23,24]]
		
		for i, name in enumerate(["Scatter", "Compute", "Reduce", "Total compute"]):
			data = np.array([confidenceInterval(getSamples(name, getMixedFilename(n, t), int(n/t)), confidence_value) for n in ns])
			plt.sca(axarr[i])
			plot_mean_and_CI(ns, data[:,0], data[:,1], data[:,2], color_mean='yellow', color_shading="yellow", linewidth=linewidth, markerSize=markerSize, label="mixed (" + str(t) + " threads per node)")

		t = 4
		#For 20
		#ns = [x*t for x in [2,6,8,9,11,12]]
		#For 50
		ns = [x*t for x in [1,3,4,5,6,8,9,11,12]]
		for i, name in enumerate(["Scatter", "Compute", "Reduce", "Total compute"]):
			data = np.array([confidenceInterval(getSamples(name, getMixedFilename(n, t), int(n/t)), confidence_value) for n in ns])
			plt.sca(axarr[i])
			plot_mean_and_CI(ns, data[:,0], data[:,1], data[:,2], color_mean='magenta', color_shading="magenta", linewidth=linewidth, markerSize=markerSize, label="mixed (" + str(t) + " threads per node)")
		t = 8
		#For 20
		#ns = [x*t for x in [3,4,6]]
		#For 50
		ns = [x*t for x in [1,2,3,4,6]]
		for i, name in enumerate(["Scatter", "Compute", "Reduce", "Total compute"]):
			data = np.array([confidenceInterval(getSamples(name, getMixedFilename(n, t), int(n/t)), confidence_value) for n in ns])
			plt.sca(axarr[i])
			plot_mean_and_CI(ns, data[:,0], data[:,1], data[:,2], color_mean='black', color_shading="black", linewidth=linewidth, markerSize=markerSize, label="mixed (" + str(t) + " threads per node)")



	ns = [i for i in range(1,25)]
	# omp only
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
	plot_mean_and_CI([i for i in range(1,25)], to_plot, u, l,color_mean="red", linewidth=linewidth, color_shading="#5790ed", label="OMP")



	# plot comm avoiding
	ns = [1,2,4,8,12,16, 24, 32,48]
	data = np.array([confidenceInterval(getComAvoidingSamples(getCommAvoidingFilename(n)), confidence_value) for n in ns])
	plt.sca(axarr[3])
	plot_mean_and_CI(ns, data[:,0], data[:,1], data[:,2], color_mean='green', color_shading="green", linewidth=linewidth, markerSize=markerSize, label="comm avoiding")

	top_max = [2,40,10,45]
	for i, ax in enumerate(axarr):
		plt.sca(axarr[i])
		plt.title(titles[i])
		plt.xlabel("Number of cores")
		plt.ylabel("Time [s]")
		plt.ylim(bottom=0, top=top_max[i])
		plt.xlim(left=0, right=48)
		plt.legend()



	fig.tight_layout()

	plt.savefig("mpi_mixtures_with_everything", dpi=40, format='eps')






#plt.show()



