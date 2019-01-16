#!/usr/bin/python3
import os
import numpy as np
import scipy.stats as st
import glob
import matplotlib.pyplot as plt
import re
from sys import argv

from matplotlib.ticker import MaxNLocator

confidence_value = 0.95
save = 0 if len(argv) <= 1 else argv[1]

# Returns point estimate, begin and end of confidence interval.
def confidenceInterval(data, prob_interval) :
    # Note: the probability of the confidence interval starting at k and -k -th data point
    #       is  1 - 2 * binom(k, len(data), 0.5)

    p = np.ones(len(data)) / len(data)
    distro = st.rv_discrete(values = (data, p))
    point = distro.median()
    interval = distro.interval(prob_interval)
    return point, interval[0], interval[1]

def get_num(string, key, label_first = 0):
    match = re.search(key + '_[0-9]+' if label_first else '[0-9]+_' + key, string).group(0)
    res = re.search('[0-9]+', match).group(0)
    return int(res)

def sortFirst(data) :
    return data[np.lexsort(np.fliplr(data).T)]

def load(filenames, separator = '\t', col = 0, dtype = np.float):
    data = []
    for filename in filenames:
        try:
            file = np.loadtxt(filename, comments='#', delimiter=separator, dtype = dtype)
            cores = get_num(filename, 'cores')
            threads = get_num(filename, 't', 1)
            procs = int(cores / threads)

            times = np.zeros(file.shape[0])
            for i in range(len(times)):
                times[i] = file[i][col] if len(file.shape) == 2 or dtype != np.float else file[i]

            point, start, end = confidenceInterval(times, confidence_value)

            data.append([procs, point, point - start, end - point])
        except:
            print("could not read", filename)

    return sortFirst(np.array(data))

def plot(data, label):
    error = np.array([data[:,2], data[:,3]])
    plt.errorbar(data[:,0], data[:,1], yerr=error, fmt='--o', label=label)

def format(n): # return an easily readable string.
    if n >= 1e3 and n < 1e6 : return str(int(n / 1000)) + 'k'
    elif n >= 1e6 and n < 1e9 : return str(int(n / 1e6)) + 'M'
    else : return str(n)

#font
font = {'family' : 'normal',
        'size'   : 12}
plt.rc('font', **font)

probs=[0.25, 0.5]#, 0.75]

fig_id = 0

for prob in probs:
    plt.figure(fig_id)

    filenames_red = glob.glob('./grid_outputs/*' + str(prob) + '_p_*reduction.txt')
    filenames_dist = glob.glob('./grid_outputs/*' + str(prob) + '_p_*distributed.txt')


    data_red = load(filenames_red)
    data_dist = load(filenames_dist)

    ax = plt.figure().gca()

    plot(data_red, 'mpi reduction')
    plot(data_dist, 'distributed')


    plt.xlabel('Number of cores')
    plt.ylabel('Time [s]')

    plt.title('Edge probability: ' + str(prob))
    plt.legend(loc='best')

    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    plt.tight_layout(pad=0.2, h_pad=0.2, w_pad=0.2)
    if save : plt.savefig('all_vertices_' + format(vertices) + '.pdf')

    fig_id += 1

if not save : plt.show()
