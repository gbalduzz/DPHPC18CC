#!/usr/bin/python3
import os
import numpy as np
import scipy.stats as st
import glob
import matplotlib.pyplot as plt
import re

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


filenames = glob.glob('./run_*_timings.txt')

data = []

for filename in filenames:
    file = np.loadtxt(filename, comments='#')
    if len(file.shape) == 1 or file.shape[0] < 3 : continue

    run = int(re.search('[0-9]+', filename).group(0))
    point, start, end = confidenceInterval(file[:,0], confidence_value)

    size = 1000 **2 * run
    data.append([size, point, point - start, end - point])

data = np.array(data)

error = np.array([data[:,2], data[:,3]])

plt.errorbar(data[:,0], data[:,1], yerr=error, fmt='o')
plt.xlabel('size')
plt.ylabel('sec')
plt.title(r'Graph scaling. $p = 0.5$')

plt.savefig('graph_scaling.pdf')
plt.show()
