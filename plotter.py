#!/usr/bin/env python

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np



xvals = []
yvals = []
zvals = []

filepath = str(input("Enter file path: "))

with open(filepath, "r") as a_file:
  for line in a_file:
    stripped_line = line.strip()
    letter_list = stripped_line.split(",")
    xvals.append(float(letter_list[0]))
    yvals.append(float(letter_list[1]))
    zvals.append(float(letter_list[2]))


fig = plt.figure()
ax = plt.axes(projection='3d')

ax.scatter3D(xvals, zvals, yvals, c=yvals)
# rotate the axes and update
for angle in range(0, 360):
    ax.view_init(30, angle)
    plt.draw()
    plt.pause(.001)
