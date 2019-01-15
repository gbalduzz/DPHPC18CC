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

def get_num(string, key):
    match = re.search('[0-9]+_' + key, string).group(0)
    res = re.search('[0-9]+', match).group(0)
    return int(res)

def sortFirst(data) :
    return data[np.lexsort(np.fliplr(data).T)]

def load(filenames, separator = '\t', col = 0, dtype = np.float, cores_key = 'procs'):
    data = []
    for filename in filenames:
        try:
            file = np.loadtxt(filename, comments='#', delimiter=separator, dtype = dtype)
            procs = get_num(filename, cores_key)

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

#font
font = {'family' : 'normal',
        'size'   : 11}
plt.rc('font', **font)

n_vertices=[500000, 1000000, 2000000]

#plot_gcc = 0
fig_id = 0

for vertices in n_vertices:
    plt.figure(fig_id)

    filenames_theirs = glob.glob('./outputs/theirs_'+ str(vertices) + '_*')
    filenames = glob.glob('./outputs/ours_cray_'+ str(vertices) + '_*')
    filenames2 = glob.glob('./outputs/ours_cray_t2_'+ str(vertices) + '_*')
    filenames4 = glob.glob('./outputs/ours_cray_t4_'+ str(vertices) + '_*')
    filenames_omp = glob.glob('./outputs/omp_cray_'+ str(vertices) + '_*')

    dtype = {'names': ('file', 'arg1', 'arg2', 'v', 'e', 'time', 'ctime','alg','comp'),
            'formats': (np.str, np.int, np.int, np.int, np.int, np.float, np.float, np.str, np.int)}

    data = load(filenames)
    data2 = load(filenames2)
    data4 = load(filenames4)
    data_omp = load(filenames_omp, cores_key='t')
    data_theirs = load(filenames_theirs, ',', 5, dtype)

    ax = plt.figure().gca()

    plot(data, 'mpi 1 thread')
    plot(data2, 'mpi 2 threads')
    plot(data4, 'mpi 4 threads')
    plot(data_omp, 'omp only')
    plot(data_theirs, 'comm avoiding')


    plt.xlabel('Cores')
    plt.ylabel('runtime [s]')

    plt.title('500M edges, ' + str(int(vertices / 1000)) + 'k vertices.')
    plt.legend(loc='best')

    ax.xaxis.set_major_locator(MaxNLocator(integer=True))

    #if save : plt.savefig(str(vertices) + '.png', format='png', dpi=900)
    if save : plt.savefig('all_vertices_' + str(vertices) + '.pdf')

    fig_id += 1

plt.show()
