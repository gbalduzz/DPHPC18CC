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
# Returns point estimate, begin and end of confidence interval.
def confidenceInterval(data, prob_interval) :
    # Note: the probability of the confidence interval starting at k and -k -th data point
    #       is  1 - 2 * binom(k, len(data), 0.5)

    p = np.ones(len(data)) / len(data)
    distro = st.rv_discrete(values = (data, p))
    point = distro.median()
    interval = distro.interval(prob_interval)
    return point, interval[0], interval[1]

def getSamplesDaint(filename, n):
    df = pd.read_csv(filename,delimiter = '\t',comment='#')
    data = df.values
        
    return data[:,0]




def getMpiOnlyFilenameDaint(n, graph):
    return "ours_cray_" +str(graph) + "_vertices_" + str(n) + "_procs.txt"


def getMpiOnlyFilename(n):
    return "mpi/mpi_" + str(n) + ".log"

def getSamples(col_name, filename, n):
    try:
        if(col_name == "Total compute"):
            values_compute = np.array(getSamples("Compute", filename, n))
            values_reduce = np.array(getSamples("Reduce", filename, n))
            return values_compute + values_reduce

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

if(doSpeedup):
    ns = [1,2,4,8,12,16,18,20,24,28,32,36,40,44,48]
    data = np.array([confidenceInterval(getSamplesDaint("outputs/" + getMpiOnlyFilenameDaint(n, 10000000), n), confidence_value) for n in ns])[:,0]
    speedups = [data[0] / x for x in data]
    
    data_2 = np.array([confidenceInterval(getSamplesDaint("outputs/" + getMpiOnlyFilenameDaint(n,500000), n), confidence_value) for n in ns])[:,0]
    speedups_2 = [data_2[0] / x for x in data_2]
    data_3 = np.array([confidenceInterval(getSamplesDaint("outputs/" + getMpiOnlyFilenameDaint(n,20000000), n), confidence_value) for n in ns])[:,0]
    speedups_3 = [data_3[0] / x for x in data_3]
    

    plt.plot(ns, speedups_3, marker='s',markersize=8,label="20'000'000 Vertices, Daint",color='firebrick')
    plt.plot(ns, speedups, marker='o', markersize=8,label="10'000'000 Vertices, Daint",color='lightsalmon')
    plt.plot(ns, speedups_2, marker='^',markersize=8,label="500'000 Vertices, Daint",color='saddlebrown')
    

    ns = [1,3,5,7,9,11,13,15,17,19,21,23,25,27,29,31,33,35,37,41,43,47]
    data = np.array([confidenceInterval(getSamples("Total compute", benchmark_dir1 + getMpiOnlyFilename(n), n), confidence_value) for n in ns])[:,0]
    speedups = [data[0] / x for x in data]
    
    data_2 = np.array([confidenceInterval(getSamples("Total compute", benchmark_dir2 + getMpiOnlyFilename(n), n), confidence_value) for n in ns])[:,0]
    speedups_2 = [data_2[0] / x for x in data_2]

    
    data_3 = np.array([confidenceInterval(getSamples("Total compute", benchmark_dir3 + getMpiOnlyFilename(n), n), confidence_value) for n in ns])[:,0]
    speedups_3 = [data_3[0] / x for x in data_3]



    plt.plot(ns, speedups_3, marker='s',markersize=8,label="20'000'000 Vertices, Euler",color='navy')
    plt.plot(ns, speedups, marker='o',markersize=8,label="10'000'000 Vertices, Euler",color='lightblue')
    plt.plot(ns, speedups_2, marker='^',markersize=8,label="500'000 Vertices, Euler",color='mediumslateblue')

    plt.title("Mpi only speed up")
    plt.xlim(0,48)
    plt.ylim(0,48)
    plt.xlabel("Number of cores")
    plt.ylabel("Speed up")

    plt.tight_layout(pad=0.1,w_pad=0.1, h_pad=0.1)
    plt.plot(ns, ns, label="Linear",color='green')
    plt.legend()
    plt.savefig(fig_dir + "mpi_speedup_with_ref.pdf", format='pdf')







plt.show()



