#!/usr/bin/env python

import copy
import math
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
import sys

from argparse import ArgumentParser
from collections import namedtuple
from datetime import datetime
from pathlib import Path
from pyquaternion import Quaternion
from scipy.spatial.distance import cosine

sns.set()
sns.set_style('whitegrid')

canopus_normal = np.array([0, 0, 1])
vega_normal = np.array([1, 0, 0])
up_vector = np.array([0, 1, 0])

colors = ['r', 'g', 'b']

ColumnIdx = namedtuple('ColumnIdx', ['person', 'device'])
Association = namedtuple('Association', ['score', 'angle'])
AssociationCount = namedtuple('Count', ['person_id', 'pd', 'canopus', 'vega'])


def parse_options():
    parser = ArgumentParser(sys.argv)
    parser.add_argument('--single-file')
    parser.add_argument('--multi-file', required=True)
    parser.add_argument('--min-score', type=float, default=0)
    parser.add_argument('--min-score-diff', type=float, default=0)
    parser.add_argument('--outdir', default=str(Path().absolute()))

    return parser.parse_args()


def compute_score(v1, v2):
    score = max(0, cosine(v1, v2) - 1)
    return Association(score, math.degrees(np.arccos(score)))


def extract(df, index):
    keys = ['{}{}'.format(c, index) for c in ['rw', 'rx', 'ry', 'rz']]
    return pd.DataFrame(data=df[keys].values, columns=['rw', 'rx', 'ry', 'rz'])


def make_vector(q):
    # Our motion capture system rotates the upvector by the quaternion.
    # So in order to get our desired vector, we take the quaternion
    # recorded by the motion capture system and rotate the up-vector
    return Quaternion(q).rotate(up_vector)


device_names = ['Personal Device', 'Canopus', 'Vega']


class ScoreComputation(object):
    columns = ['person_id',
               'pd_score',
               'pd_angle',
               'canopus_score',
               'canopus_angle',
               'vega_score',
               'vega_angle',
               'max_assoc_name',
               'max_assoc_score',
               'binary_assoc_name',
               'watching_public_display']

    def __init__(self, df, person_id, column_idx, **kwargs):
        self.__table = []
        self.__person_id = person_id
        self.__column_idx = column_idx
        self.__src_df = df
        self.__min_score = kwargs['min_score']
        self.__min_score_diff = kwargs['min_score_diff']
        self.__compute()

    def __add(self, *args):
        cosine_scores = np.array([arg.score for arg in args])
        sorted_scores = copy.deepcopy(cosine_scores)
        sorted_scores = -np.sort(-sorted_scores)

        if sorted_scores[0] < self.__min_score:
            return

        if abs(sorted_scores[0] - sorted_scores[1]) < self.__min_score_diff:
            return

        idx = np.argmax(cosine_scores)
        device = device_names[idx]
        binary_assoc = device if idx == 0 else 'Public Display'
        watching_public_display = min(1, int(idx))
        row = [self.__person_id] + \
              [s for arg in args for s in [arg.score, arg.angle]] + \
              [device, cosine_scores[idx], binary_assoc,
               watching_public_display]
        self.__table.append(row)
        return cosine_scores

    def __compute(self):
        person = extract(self.__src_df, self.__column_idx.person)
        device = extract(self.__src_df, self.__column_idx.device)

        counts = [0, 0, 0]

        for i in range(0, len(person)):
            head_vector = make_vector(person.values[i])
            device_vector = make_vector(device.values[i])

            pd = compute_score(head_vector, device_vector)
            canopus = compute_score(head_vector, canopus_normal)
            vega = compute_score(head_vector, vega_normal)

            scores = self.__add(pd, canopus, vega)
            max_assoc_idx = np.argmax(scores)

            counts[max_assoc_idx] += 1

        self.__assoc_counts = AssociationCount(person_id=self.__person_id,
                                               pd=counts[0],
                                               canopus=counts[1],
                                               vega=counts[2])

    @property
    def person_id(self):
        return self.__person_id

    @property
    def df(self):
        return pd.DataFrame(data=self.__table, columns=ScoreComputation.columns)

    @property
    def association_counts(self):
        return self.__assoc_counts

    def __add__(self, other):
        return self.df.append(other.df)


def make_combined_data(df, min_score, min_score_diff):
    p1 = ScoreComputation(df, 'Person 1', ColumnIdx(person=1, device=2),
                          min_score=min_score, min_score_diff=min_score_diff)
    p2 = ScoreComputation(df, 'Person 2', ColumnIdx(person=3, device=4),
                          min_score=min_score, min_score_diff=min_score_diff)
    p3 = ScoreComputation(df, 'Person 3', ColumnIdx(person=5, device=6),
                          min_score=min_score, min_score_diff=min_score_diff)
    all_data = p1.df.append(p2.df).append(p3.df)
    return [p1, p2, p3], all_data


def tick_labels():
    return ['{0:.1f}'.format(x) for x in np.arange(0, 1.1, 0.1)]


class Plotter(object):
    def __init__(self, x_label, y_label, **kwargs):
        self.__x_label = x_label
        self.__y_label = y_label
        self.__title = '' if 'title' not in kwargs else kwargs['title']
        self.__fig, self.__ax = plt.subplots(figsize=(10, 10))
        self.__ax.set_xticks(np.arange(0, 1.1, 0.1))
        self.__ax.set_xticklabels(tick_labels())
        self.__ax.set_yticks(np.arange(0, 1.1, 0.1))
        self.__ax.set_yticklabels(tick_labels())

    def plot(self, data_sets, outfile_path):
        for i, data_set in enumerate(data_sets):
            sns.scatterplot(x=self.__x_label, y=self.__y_label,
                            data=data_set.df, color=colors[i],
                            label=data_set.person_id)
        plt.title(self.__title)
        plt.legend(bbox_to_anchor=[1.2, 1])
        plt.tight_layout()
        plt.savefig(outfile_path)


def make_multi_scatter(options, x_label, y_label, **kwargs):
    title = '{} vs {}'.format(y_label.capitalize(), x_label.capitalize()) \
        if 'title' not in kwargs else kwargs['title']
    x_data_label = '{}_score'.format(x_label)
    y_data_label = '{}_score'.format(y_label)

    separate, _ = make_combined_data(kwargs['df'], options.min_score,
                                     options.min_score_diff)
    plotter = Plotter(x_data_label, y_data_label, title=title)

    outfile_name = kwargs['outdir'] / '{}_{}.png'.format(y_label, x_label)
    plotter.plot(separate, outfile_name)


class DiscreteCounter(object):
    def __init__(self, *args):
        self.__counts = [arg for arg in args]
        self.__table = []

    def make_table(self):
        for i, count in enumerate(self.__counts):
            total = DiscreteCounter.sum_counts(count)
            all_counts = [count.pd, count.canopus, count.vega]
            all_percents = [
                DiscreteCounter.percent(count.pd, total),
                DiscreteCounter.percent(count.canopus, total),
                DiscreteCounter.percent(count.vega, total)
            ]
            max_count_index = np.argmax(all_counts)
            max_device_name = device_names[max_count_index]
            max_device_count = all_counts[max_count_index]
            max_device_percentage = all_percents[max_count_index]
            self.__table.append([
                count.person_id,
                count.pd, all_percents[device_names.index('Personal Device')],
                count.canopus, all_percents[device_names.index('Canopus')],
                count.vega, all_percents[device_names.index('Vega')],
                max_device_name,
                max_device_count,
                max_device_percentage
            ])

    @property
    def data_frame(self):
        columns = ['Person ID', 'Personal Device Count',
                   'Personal Device %',
                   'Canopus Count', 'Canopus %', 'Vega Count', 'Vega %',
                   'Max Device Name', 'Max Device Assoc Count',
                   'Max Device Assoc %']
        return pd.DataFrame(data=self.__table, columns=columns)

    @staticmethod
    def percent(n, d):
        if d == 0:
            print('denominator 0, returning 0')
            return 0
        return float(n) * 100.0 / d

    @staticmethod
    def sum_counts(count):
        return count.pd + count.canopus + count.vega


def draw_count_plots(options, df, title, **kwargs):
    separate, all_data = make_combined_data(df, options.min_score,
                                            options.min_score_diff)
    discrete_counter = DiscreteCounter(separate[0].association_counts,
                                       separate[1].association_counts,
                                       separate[2].association_counts)
    discrete_counter.make_table()
    ax = discrete_counter.data_frame.plot(
        x='Person ID',
        y=['Personal Device Count', 'Canopus Count', 'Vega Count'],
        kind='bar', title=title, figsize=(8, 6)
    )
    ax.set_ylabel('Counts')
    plt.tight_layout()
    plt.savefig(kwargs['filepath'])


def draw_binary_plots(options, df, title, **kwargs):
    s, d = make_combined_data(df, options.min_score, options.min_score_diff)
    dc = DiscreteCounter(s[0].association_counts,
                         s[1].association_counts,
                         s[2].association_counts)
    dc.make_table()
    df = dc.data_frame
    pdc_col = df[['Canopus Count', 'Vega Count']].sum(axis=1)
    df['Public Display Count'] = pdc_col
    ax = df.plot(x='Person ID',
                 y=['Personal Device Count', 'Public Display Count'],
                 kind='bar', title=title, figsize=(8, 6))
    ax.set_ylabel('Counts')
    plt.tight_layout()
    plt.savefig(kwargs['filepath'])


def main():
    options = parse_options()
    outdir = Path(options.outdir) / datetime.now().strftime('%Y%m%d-%H%M%S')
    outdir.mkdir(parents=True, exist_ok=True)

    multi_df = pd.read_csv(options.multi_file)

    make_multi_scatter(options, 'canopus', 'vega', outdir=outdir, df=multi_df)
    make_multi_scatter(options, 'canopus', 'pd', df=multi_df,
                       title='Personal Device vs Canopus', outdir=outdir)
    make_multi_scatter(options, 'vega', 'pd', df=multi_df,
                       title='Personal Device vs Vega', outdir=outdir)

    draw_count_plots(options, multi_df,
                     filepath=outdir / 'multi-associations.png',
                     title='Multi-Screen Associations (Separate)')
    draw_binary_plots(options, multi_df,
                      filepath=outdir / 'multi-binary_associations.png',
                      title='Multi-Screen Associations (Binary)')

    single_df = pd.read_csv(options.single_file)
    draw_binary_plots(options, single_df,
                      filepath=outdir / 'single-binary_associations.png',
                      title='Single-Screen Associations (Binary)')


if __name__ == '__main__':
    main()
