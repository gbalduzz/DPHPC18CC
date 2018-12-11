#!/usr/bin/python3
import os

####### EDIT   ######
tasks = [1, 2, 4, 8, 12, 16]
threads = 18
size_x = 20000
submit = False
#####################

iter_id = 0

for task in tasks:
    text = open('job_daint.pbs.in', 'r').read()

    text = text.replace('__PROCS__', str(task))
    text = text.replace('__THREADS__', str(threads))
    text = text.replace('__SIZE_X__', str(size_x))
    text = text.replace('__SIZE_Y__', str(size_x * task))
    text = text.replace('__OUTNAME__', 'procs_' + str(task) + 'timings.txt')

    outname = 'job_procs_' + str(task)+ '.pbs'
    job_file = open(outname, 'w')
    job_file.write(text)

    if submit : os.system('sbatch ' + outname)


