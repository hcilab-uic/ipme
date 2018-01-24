#!/usr/bin/env python3


import csv
import math
import os
import sys
# import quaternion

import plotly.plotly as py
import plotly.graph_objs as go
import plotly.figure_factory as FF

import numpy as np
import pandas as pd

from argparse import ArgumentParser
from numpy import genfromtxt
# from matplotlib import pyplot as plt
from sklearn.cluster import KMeans
from pyquaternion import Quaternion


filepath = './frames.csv'
participants_in_frame = 1

df = pd.read_csv(filepath, header=None)
data = genfromtxt(filepath, delimiter=',')

head_1 = data[0][1:8]
position = head_1[0:3]
rotation = head_1[3:7]

col_count = len(data[0][1:])
cols_per_marker = 7
marker_count = int(col_count / cols_per_marker)

output_table = [['seconds','timestamp'] +
                ['x', 'y', 'z', 'radians', 'rot-axis-1', 'rot-axis-2',
                 'rot-axis-3'] * marker_count]

# output_table = [['timestamp'] +
#                 ['x', 'y', 'z', 'radians', 'rot-axis-1', 'rot-axis-2',
#                  'rot-axis-3'] * marker_count]

row_idx = 0
start_timestamp = None
for row in data:
    row_idx += 1
    full_row = [int(row[0])]
    for i in range(0, marker_count):
        col_start = 1 + (i * cols_per_marker)
        col_end = col_start + cols_per_marker
        
        pos_start = col_start
        pos_end = col_start + 3
        rot_start = pos_end
        rot_end = rot_start + 4
        output_row = row[pos_start : pos_end].tolist()

        q = Quaternion(row[rot_start : rot_end])
        output_row.append(q.angle)
        output_row += q.get_axis().tolist()
        full_row += output_row

    if not start_timestamp:
        start_timestamp = int(full_row[0])
    seconds = (int(full_row[0]) - start_timestamp) / 1000

    output_table.append([seconds] + full_row)


with open('frames_with_angles.csv', 'w') as f:
    writer = csv.writer(f)
    writer.writerows(output_table)
