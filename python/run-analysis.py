#!/usr/bin/env python

import copy
import logging
import math
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import scipy.stats as stats
import seaborn as sns
import sys

from argparse import ArgumentParser
from collections import namedtuple
from datetime import datetime
from pathlib import Path
from pyquaternion import Quaternion
from scipy.spatial.distance import cosine
from scipy.stats import mannwhitneyu, shapiro, ttest_ind, ranksums


logger = logging.getLogger(__name__)
FORMAT = '%(asctime)-15s %(levelname)s %(funcName)s:%(lineno)s %(message)s'
logging.basicConfig(format=FORMAT)

sns.set()
sns.set_style('whitegrid')

canopus_normal = np.array([0, 0, 1])
vega_normal = np.array([1, 0, 0])
up_vector = np.array([0, 1, 0])

colors = ['r', 'g', 'b']

ColumnIdx = namedtuple('ColumnIdx', ['person', 'device'])
Association = namedtuple('Association', ['score', 'angle'])
AssociationCount = namedtuple('Count', ['person_id', 'pd', 'canopus', 'vega'])
CombinedData = namedtuple('CombinedData', ['separate', 'all', 'timestamps',
                                           'time_slices', 'stats'])


def parse_options():
    parser = ArgumentParser(sys.argv)
    parser.add_argument('--single-file')
    parser.add_argument('--multi-file', required=True)
    parser.add_argument('--min-score', type=float, default=0)
    parser.add_argument('--min-score-diff', type=float, default=0)
    parser.add_argument('--outdir', default=str(Path().absolute()))
    parser.add_argument('--title')
    parser.add_argument('--log-level', default='info')
    parser.add_argument('--hist-range-min', default=0.0, type=float)
    parser.add_argument('--hist-range-max', default=30000, type=float)
    parser.add_argument('--bin-count', default=25)
    parser.add_argument('--fig-width', type=int, default=10)
    parser.add_argument('--fig-height', type=int, default=8)

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
        self.__time_spent = [0, 0, 0]
        self.__time_slices = []
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
        timestamps = self.__src_df['timestamp']

        counts = [0, 0, 0]

        previous_assoc_idx = np.inf
        previous_ts = 0
        for i in range(0, len(person)):
            head_vector = make_vector(person.values[i])
            device_vector = make_vector(device.values[i])

            personal_device = compute_score(head_vector, device_vector)
            canopus = compute_score(head_vector, canopus_normal)
            vega = compute_score(head_vector, vega_normal)

            scores = self.__add(personal_device, canopus, vega)
            max_assoc_idx = np.argmax(scores)

            if previous_assoc_idx == np.inf:
                previous_assoc_idx = max_assoc_idx

            if max_assoc_idx != previous_assoc_idx:
                time_delta = timestamps[i] - previous_ts
                self.__time_spent[previous_assoc_idx] += time_delta

                slices = [0, 0, 0]
                slices[previous_assoc_idx] = np.log(time_delta)
                self.__time_slices.append([self.__person_id] + slices)

                logger.debug('switched from {} to {}, td={}ms'.format(
                    previous_assoc_idx, max_assoc_idx, time_delta))
                previous_assoc_idx = max_assoc_idx
                previous_ts = timestamps[i]

            counts[max_assoc_idx] += 1

        self.__assoc_counts = AssociationCount(
            person_id=self.__person_id, pd=counts[0], canopus=counts[1],
            vega=counts[2])

    @property
    def time_slices(self):
        return pd.DataFrame(data=self.__time_slices,
                            columns=['Person ID', 'Personal Device',
                                     'Canopus', 'Vega'])

    @property
    def time_spent(self):
        return [self.__person_id] + self.__time_spent

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


def make_combined_data(df, min_score, min_score_diff, use_vega=True):
    p1 = ScoreComputation(df, 'Person 1', ColumnIdx(person=1, device=2),
                          min_score=min_score, min_score_diff=min_score_diff)
    p2 = ScoreComputation(df, 'Person 2', ColumnIdx(person=3, device=4),
                          min_score=min_score, min_score_diff=min_score_diff)
    p3 = ScoreComputation(df, 'Person 3', ColumnIdx(person=5, device=6),
                          min_score=min_score, min_score_diff=min_score_diff)
    all_data = p1.df.append(p2.df).append(p3.df)

    time_stamp = [p1.time_spent, p2.time_spent, p3.time_spent]
    cols = ['Person ID', 'Personal Device', 'Canopus', 'Vega']
    df_time_stamp = pd.DataFrame(data=time_stamp, columns=cols)
    time_slices = p1.time_slices.append(p2.time_slices).append(p3.time_slices)

    # Perform normality testing
    def perform_normality_test(name):
        times = time_slices[name]
        w, pvalue = shapiro(times)
        logger.info('Normality test for {name}, W={W}, pvalue={pvalue}'.format(
            name=name, W=w, pvalue=pvalue))
        return times

    personal_devices = perform_normality_test('Personal Device')
    canopus_devices = perform_normality_test('Canopus')
    vega_devices = perform_normality_test('Vega')

    stat = stats.f_oneway(personal_devices, canopus_devices, vega_devices)

    return CombinedData([p1, p2, p3], all_data, df_time_stamp, time_slices,
                        stat)


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


def make_multi_scatter(x_label, y_label, **kwargs):
    title = '{} vs {}'.format(y_label.capitalize(), x_label.capitalize()) \
        if 'title' not in kwargs else kwargs['title']
    x_data_label = '{}_score'.format(x_label)
    y_data_label = '{}_score'.format(y_label)

    plotter = Plotter(x_data_label, y_data_label, title=title)

    outfile_name = kwargs['outdir'] / '{}_{}.png'.format(y_label, x_label)
    plotter.plot(kwargs['df_list'], outfile_name)


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
                DiscreteCounter.percent(count.vega, total)]
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


def draw_count_plots(df_list, title, **kwargs):
    discrete_counter = DiscreteCounter(df_list[0].association_counts,
                                       df_list[1].association_counts,
                                       df_list[2].association_counts)
    discrete_counter.make_table()
    ax = discrete_counter.data_frame.plot(
        x='Person ID',
        y=['Personal Device Count', 'Canopus Count', 'Vega Count'],
        kind='bar', title=title, figsize=(8, 6)
    )
    ax.set_ylabel('Counts')
    plt.tight_layout()
    plt.savefig(kwargs['filepath'])


def draw_binary_plots(df_list, title, **kwargs):
    dc = DiscreteCounter(df_list[0].association_counts,
                         df_list[1].association_counts,
                         df_list[2].association_counts)
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
    logger.setLevel(logging.getLevelName(options.log_level.upper()))

    dir_name = datetime.now().strftime('%Y%m%d-%H%M%S')
    if options.title:
        dir_name += '-{}'.format(options.title)

    outdir = Path(options.outdir) / dir_name
    outdir.mkdir(parents=True, exist_ok=True)

    multi_df = pd.read_csv(options.multi_file)
    multi = make_combined_data(multi_df, options.min_score,
                               options.min_score_diff)

    make_multi_scatter('canopus', 'vega', outdir=outdir, df_list=multi.separate)
    make_multi_scatter('canopus', 'pd', df_list=multi.separate, outdir=outdir,
                       title='Personal Device vs Canopus')
    make_multi_scatter('vega', 'pd', df_list=multi.separate, outdir=outdir,
                       title='Personal Device vs Vega')

    draw_count_plots(multi.separate,
                     filepath=outdir / 'multi-associations.png',
                     title='Multi-Screen Associations (Separate)')
    draw_binary_plots(multi.separate,
                      filepath=outdir / 'multi-binary_associations.png',
                      title='Multi-Screen Associations (Binary)')

    single_df = pd.read_csv(options.single_file)
    single = make_combined_data(single_df, options.min_score,
                                options.min_score_diff, use_vega=False)
    draw_binary_plots(single.separate,
                      filepath=outdir / 'single-binary_associations.png',
                      title='Single-Screen Associations (Binary)')

    figsize = (options.fig_width, options.fig_height)
    hist_multi = multi.time_slices.hist(
        bins=int(options.bin_count),
        range=(options.hist_range_min, options.hist_range_max), figsize=figsize)
    plt.savefig(outdir / 'multi_hist.png')
    multi.time_slices.to_csv(outdir / 'multi_time_slices.csv')
    logger.info('Multi-screen ANOVA F-Statistic {}, p-value {}'.format(
        multi.stats.statistic, multi.stats.pvalue))

    hist_single = single.time_slices.hist(
        bins=int(options.bin_count),
        range=(options.hist_range_min, options.hist_range_max), figsize=figsize)

    plt.savefig(outdir / 'single_hist.png')

    def report_tests(name):
        x = multi.time_slices[name]
        y = single.time_slices[name]
        t_stat, t_pvalue = ttest_ind(x, y)
        logger.info('{name}: t-test={stat}, pvalue={pvalue}'.format(
            name=name, stat=t_stat, pvalue=t_pvalue))
        u_stat, u_pvalue = mannwhitneyu(x, y)
        logger.info('{name}: Mann-Whitney statistic={stat}, '
                    'pvalue={pvalue}'.format(name=name, stat=u_stat,
                                             pvalue=u_pvalue))
        rs_stat, rs_pvalue = ranksums(x, y)
        logger.info('{name}: Wilcoxon rank-sum statistic={stat}, '
                    'pvalue={pvalue}'.format(name=name, stat=rs_stat,
                                             pvalue=rs_pvalue))

    report_tests('Personal Device')
    report_tests('Canopus')


if __name__ == '__main__':
    main()
