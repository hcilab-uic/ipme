#!/usr/bin/env python

import sys
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

from argparse import ArgumentParser
from pyquaternion import Quaternion


canopus_normal = np.array([0, 0, 1])
right_normal = np.array([-1, 0, 0])
up_vector = np.array([0, 1, 0])

adjustment_vector = Quaternion(axis=[1, 0, 0], degrees=30)


def parse_options():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('--input-file')
    parser.add_argument('--output-file')
    parser.add_argument('--col-names', required=True)

    return parser.parse_args()


def make_vector(e, apply=None):
    # Our motion capture system rotates the upvector by the quaternion.
    # So in order to get our desired vector, we take the quaternion
    # recorded by the motion capture system and rotate the up-vector
    # logger.debug(q)
    q = Quaternion(e)
    if apply:
        q *= apply
    return q.rotate(up_vector)


def compute_score(v1, v2):
    # score = max(0, cosine(v1, v2) - 1)
    score = max(0, -(v1.dot(v2)))
    return score


def main():
    options = parse_options()
    df = pd.read_csv(options.input_file)
    colnames = ['timestamp'] + options.col_names.split(',')
    data = df[colnames]
    canopus_times = []
    right_times = []

    previous_max = 0
    previous_ts = 0
    for i, row in enumerate(data.values):
        u = make_vector(row[1:], adjustment_vector)
        canopus_score = compute_score(u, canopus_normal)
        right_score = compute_score(u, right_normal)

        current_max = 'canopus' if canopus_score > right_score else 'right'
        # print(current_max)
        if i == 0:
            previous_max = current_max

        if current_max != previous_max:
            # print('switched from {} to {}'.format(previous_max, current_max))
            time_delta = row[0] - previous_ts
            if previous_max == 'canopus':
                canopus_times.append(time_delta)
            elif previous_max == 'right':
                right_times.append(time_delta)
            previous_max = current_max
            previous_ts = row[0]

    last_delta = data.values[-1][0] - previous_ts
    if previous_max == 'canopus':
        canopus_times.append(last_delta)

    if len(canopus_times) < len(right_times):
        right_times = right_times[1:]
    elif len(right_times) < len(canopus_times):
        canopus_times = canopus_times[1:]

    print('{}, {}'.format(len(canopus_times), len(right_times)))
    table = []
    for i in range(len(right_times)):
        table.append([canopus_times[i], right_times[i]])

    if options.output_file:
        pd.DataFrame(data=table, columns=['Canopus', 'Right Wall']).to_csv(
            options.output_file, index=False)


if __name__ == '__main__':
    main()
