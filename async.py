#!/usr/bin/env python3

import os
import sys
import time
import async

c_start = time.clock()
w_start = time.time()

print ("(Python) Starting loop...")

flist = []
for i in range(0, 100):
    print ("(Python) Running async iteration %i..." % i)
    fut = async.run()
    flist.append(fut)

print ("(Python) Finished with C++ code...")

n = 0
sum = 0
for fut in flist:
    print ("(Python) Getting future for %i..." % n)
    sum += fut.get()
    n += 1

c_end = time.clock()
w_end = time.time()

c_diff = c_end - c_start 
w_diff = w_end - w_start
cpu_usage = (c_diff / w_diff) * 100.

print ("")
print ("(Python) Sum = %i" % sum)
print ("")
print ("CPU time : %f seconds" % c_diff)
print ("Wall time : %f seconds" % w_diff)
print ("")
print ("%s CPU : %6.2f%s" % ('%', cpu_usage, '%'))

                            