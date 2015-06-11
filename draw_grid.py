#!/usr/bin/env python3

import sys
import numpy

inf = open(sys.argv[1], 'r')
outf = open(sys.argv[2], 'r')

nodes = int(inf.readline().split(" ")[0])

edges = numpy.zeros((nodes, nodes))


for i in range(0, nodes):
    nums = inf.readline().split()[1:]
    for j in zip(nums[::2], nums[1::2]):
        edges[i][int(j[0])] = int(j[1])

outf.readline()
for i in range(0, nodes):
    nums = outf.readline().split()[1:]
    for j in zip(nums[::2], nums[1::2]):
        edges[i][int(j[0])] = -int(j[1])

print("graph g {")

for i in range(0, nodes):
    print("n", i, ";",  sep="")

for i in range(0, nodes):
    for j in range(i, nodes):
        cost = int(edges[i][j])
        if (cost > 0):
            print("n", i, " -- ", "n", j, " [label=", cost, ", len=2];", sep="")
        elif (cost < 0):
            print("n", i, " -- ", "n", j, " [color=red, penwidth=10, len=2, label=", -cost, "];", sep="")

print("}")
