#!/usr/bin/python3

####### EDIT   ######
tasks = [1, 2, 4, 8, 12 16]
threads = 18
#####################

iter_id = 0

for task in tasks:
    text = open('job_daint.pbs.in', 'r').file.read()

    text = text.replace('BETA', str(beta))


