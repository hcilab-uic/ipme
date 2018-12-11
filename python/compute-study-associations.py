#!/usr/bin/env python

import sys
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

from argparse import ArgumentParser
from pyquaternion import Quaternion


canopus_normal = np.array([0, 0, 1])
vega_normal = np.array([1, 0, 0])
up_vector = np.array([0, 1, 0])

adjustment_vector = Quaternion(axis=[1, 0, 0], degrees=30)


def parse_options():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('--input-file')
    parser.add_argument('--output-file')
    parser.add_argument('--user-cols', required=True)
    parser.add_argument('--device-cols', required=True)

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
    colnames = ['timestamp'] + options.user_cols.split(',') +  \
                   options.device_cols.split(',')
    data = df[colnames]
    public_times = []
    personal_times = []

    previous_max = 0
    previous_ts = 0
    for i, row in enumerate(data.values):
        u = make_vector(row[1:5], adjustment_vector)
        v = make_vector(row[5:9])

        device_score = compute_score(u, v)
        canopus_score = compute_score(u, canopus_normal)
        vega_score = compute_score(u, vega_normal)

        current_max = 'personal' if (device_score > canopus_score and
                                     device_score > vega_score) else 'public'
        if i == 0:
            previous_max = current_max

        if current_max != previous_max:
            # print('switched from {} to {}'.format(previous_max, current_max))
            time_delta = row[0] - previous_ts
            if previous_max == 'personal':
                public_times.append(time_delta)
            elif previous_max == 'public':
                personal_times.append(time_delta)
            previous_max = current_max
            previous_ts = row[0]

    last_delta = data.values[-1][0] - previous_ts
    if previous_max == 'public':
        public_times.append(last_delta)
    elif previous_max == 'personal':
        personal_times.append(last_delta)

    if len(public_times) < len(personal_times):
        personal_times = personal_times[-len(public_times):]
    elif len(personal_times) < len(public_times):
        public_times = public_times[-len(personal_times):]

    print('{}, {}'.format(len(public_times), len(personal_times)))
    table = []
    for i in range(len(personal_times)):
        try:
            table.append([public_times[i], personal_times[i]])
        except IndexError as err:
            print(public_times, personal_times)

    print('table rows {}'.format(len(table)))

    if options.output_file:
        pd.DataFrame(data=table, columns=['Public Device',
                                          'Personal Device']).to_csv(
            options.output_file, index=False)


if __name__ == '__main__':
    main()
