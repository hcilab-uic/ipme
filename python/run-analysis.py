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
from scipy.stats import mannwhitneyu, shapiro, ttest_ind, ranksums, ttest_rel
from statsmodels.graphics.gofplots import qqplot

from utils.log import setup_logger, set_log_level

logger = setup_logger(__name__)

sns.set()
sns.set_style('whitegrid')

canopus_normal = np.array([0, 0, 1])
vega_normal = np.array([1, 0, 0])
up_vector = np.array([0, 1, 0])

colors = ['r', 'g', 'b']

ColumnIdx = namedtuple('ColumnIdx', ['person', 'device'])
Association = namedtuple('Association', ['score', 'angle'])
AssociationCount = namedtuple('Count', ['person_id', 'pd', 'canopus', 'vega'])
# CombinedData = namedtuple('CombinedData', ['separate', 'all', 'timestamps',
#                                            'time_slices', 'stats'])
CombinedData = namedtuple('CombinedData', ['separate', 'all',
                                           'time_slices_table'])


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
    parser.add_argument('--transform-type', default='none',
                        choices=['none', '2', 'natural', '10'])
    parser.add_argument('--alpha', default=0.01)

    return parser.parse_args()


def compute_score(v1, v2):
    # score = max(0, cosine(v1, v2) - 1)
    score = max(0, -(v1.dot(v2)))
    return Association(score, math.degrees(np.arccos(score)))


def extract(df, index):
    keys = ['{}{}'.format(c, index) for c in ['rw', 'rx', 'ry', 'rz']]
    return pd.DataFrame(data=df[keys].values, columns=['rw', 'rx', 'ry', 'rz'])


def make_vector(q):
    # Our motion capture system rotates the upvector by the quaternion.
    # So in order to get our desired vector, we take the quaternion
    # recorded by the motion capture system and rotate the up-vector
    # logger.debug(q)
    return Quaternion(q).rotate(up_vector)


# device_names = ['Personal Device', 'Canopus', 'Vega']
device_names = ['Personal Device', 'Public Device']


def compute_transform(value, transform_type):
    if transform_type == 'none':
        return value
    if transform_type == '2':
        return np.log2(value)
    if transform_type == '10':
        return np.log10(value)
    if transform_type == 'natural':
        return np.log(value)
    raise Exception('unknown transform type {}'.format(transform_type))


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
        self.__o = kwargs['options']
        self.__min_score = self.__o.min_score
        self.__min_score_diff = self.__o.min_score_diff
        self.__use_vega = True if 'vega' not in kwargs else kwargs['vega']
        self.__time_spent = [0, 0, 0]
        self.__time_slices = []

        self.__prev_assoc_idx = np.inf
        self.__prev_ts = 0

        self.__assoc_counts = [0, 0, 0]
        self.__compute()

    def __add(self, timestamp, *args):
        cosine_scores = np.array([arg.score for arg in args])
        sorted_scores = copy.deepcopy(cosine_scores)
        sorted_scores = -np.sort(-sorted_scores)

        if sorted_scores[0] < self.__min_score:
            return

        if abs(sorted_scores[0] - sorted_scores[1]) < self.__min_score_diff:
            return

        idx = np.argmax(cosine_scores)

        if not self.__use_vega and idx == 2:
            logger.debug('VEGA not in use and found VEGA index, ignoring, '
                         'cosine scores: [{}]'.format(
                           ' '.join([format(s, '.4f') for s in cosine_scores])))
            return

        # This is not very smart, fix this
        if idx == 0:
            device = 'Personal Device'
        elif idx == 1:
            device = 'Canopus'
        elif idx == 2:
            device = 'Vega'
        else:
            raise Exception('Invalid index={idx}'.format(idx=idx))

        binary_assoc = device if idx == 0 else 'Public Display'
        watching_public_display = min(1, int(idx))
        row = [self.__person_id] + \
              [s for arg in args for s in [arg.score, arg.angle]] + \
              [device, cosine_scores[idx], binary_assoc,
               watching_public_display]

        if self.__prev_assoc_idx == np.inf:
            self.__prev_assoc_idx = idx

        if idx != self.__prev_assoc_idx:
            time_delta = timestamp - self.__prev_ts
            logger.debug('switched from {} to {}, td={}ms'.format(
                        self.__prev_assoc_idx, idx, time_delta))

            self.__time_spent[self.__prev_assoc_idx] += time_delta

            binary_slices = [0, 0]
            slice_index = min(1, int(self.__prev_assoc_idx))
            binary_slices[slice_index] = compute_transform(
                time_delta, self.__o.transform_type)
            self.__time_slices.append([timestamp, self.__person_id] +
                                      binary_slices)

            self.__prev_assoc_idx = idx
            self.__prev_ts = timestamp

        self.__assoc_counts[idx] += 1

        self.__table.append(row)

        return cosine_scores

    def __compute(self):
        person = extract(self.__src_df, self.__column_idx.person)
        device = extract(self.__src_df, self.__column_idx.device)
        timestamps = self.__src_df['timestamp']

        for i in range(0, len(person)):
            head_vector = make_vector(person.values[i])
            device_vector = make_vector(device.values[i])

            personal = compute_score(head_vector, device_vector)
            canopus = compute_score(head_vector, canopus_normal)
            vega = compute_score(head_vector, vega_normal)

            self.__add(timestamps[i], personal, canopus, vega)

        self.__assoc_counts = AssociationCount(
            person_id=self.__person_id, pd=self.__assoc_counts[0],
            canopus=self.__assoc_counts[1], vega=self.__assoc_counts[2])

    @property
    def time_slices_table(self):
        return pd.DataFrame(data=self.__time_slices,
                            columns=['Timestamp', 'Person ID',
                                     'Personal Device', 'Public Device'])

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


def test_nomality(df, name, alpha, msg_tag):
    times = df[df[name] != 0][name]
    logger.debug('Performing Shapiro-Wilk test on {name}({tag}), '
                 'length={len}'.format(name=name, len=len(times),
                                       tag=msg_tag))
    w, pvalue = shapiro(times)
    normal = False if pvalue < alpha else True
    logger.info('Normality test for {name}({tag}), W={W}, pvalue={pvalue:.4f}, '
                'normal={normal}'.format(name=name, W=w, pvalue=pvalue,
                                         normal=normal, tag=msg_tag))
    return times, normal


def make_combined_data(df, options, use_vega=True):
    p1 = ScoreComputation(df, 'Person 1', ColumnIdx(person=1, device=2),
                          options=options, vega=use_vega)
    p2 = ScoreComputation(df, 'Person 2', ColumnIdx(person=3, device=4),
                          options=options, vega=use_vega)
    p3 = ScoreComputation(df, 'Person 3', ColumnIdx(person=5, device=6),
                          options=options, vega=use_vega)
    all_data = p1.df.append(p2.df).append(p3.df)

    # time_stamp = [p1.time_spent, p2.time_spent, p3.time_spent]
    # # cols = ['Person ID', 'Personal Device', 'Canopus', 'Vega']
    # cols = ['Person ID', 'Personal Device', 'Public Device']
    # df_time_stamp = pd.DataFrame(data=time_stamp, columns=cols)
    time_slices = p1.time_slices_table.append(p2.time_slices_table).append(p3.time_slices_table)

    # Perform normality testing

    # logger.debug(time_slices)
    # personal_times, personal_normal = test_nomality(
    #     time_slices, 'Personal Device', options.alpha)
    # public_times, public_normal = test_nomality(
    #     time_slices, 'Public Device', options.alpha)
    # canopus_devices = perform_normality_test('Canopus')
    # vega_devices = perform_normality_test('Vega')

    # stat = stats.f_oneway(personal_devices, canopus_devices, vega_devices)

    return CombinedData([p1, p2, p3], all_data, time_slices)


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

#
# class DiscreteCounter(object):
#     def __init__(self, *args):
#         self.__counts = [arg for arg in args]
#         self.__table = []
#
#     def make_table(self):
#         for i, count in enumerate(self.__counts):
#             total = DiscreteCounter.sum_counts(count)
#             all_counts = [count.pd, count.canopus, count.vega]
#             all_percents = [
#                 DiscreteCounter.percent(count.pd, total),
#                 DiscreteCounter.percent(count.canopus, total),
#                 DiscreteCounter.percent(count.vega, total)]
#             max_count_index = np.argmax(all_counts)
#             max_device_name = device_names[max_count_index]
#             max_device_count = all_counts[max_count_index]
#             max_device_percentage = all_percents[max_count_index]
#             self.__table.append([
#                 count.person_id,
#                 count.pd, all_percents[device_names.index('Personal Device')],
#
#                 # count.canopus, all_percents[device_names.index('Canopus')],
#                 # count.vega, all_percents[device_names.index('Vega')],
#                 max_device_name,
#                 max_device_count,
#                 max_device_percentage
#             ])
#
#     @property
#     def data_frame(self):
#         columns = ['Person ID', 'Personal Device Count',
#                    'Personal Device %',
#                    'Canopus Count', 'Canopus %', 'Vega Count', 'Vega %',
#                    'Max Device Name', 'Max Device Assoc Count',
#                    'Max Device Assoc %']
#         return pd.DataFrame(data=self.__table, columns=columns)
#
#     @staticmethod
#     def percent(n, d):
#         if d == 0:
#             print('denominator 0, returning 0')
#             return 0
#         return float(n) * 100.0 / d
#
#     @staticmethod
#     def sum_counts(count):
#         return count.pd + count.canopus + count.vega
#
#
# def draw_count_plots(df_list, title, **kwargs):
#     discrete_counter = DiscreteCounter(df_list[0].association_counts,
#                                        df_list[1].association_counts,
#                                        df_list[2].association_counts)
#     discrete_counter.make_table()
#     ax = discrete_counter.data_frame.plot(
#         x='Person ID',
#         y=['Personal Device Count', 'Canopus Count', 'Vega Count'],
#         kind='bar', title=title, figsize=(8, 6)
#     )
#     ax.set_ylabel('Counts')
#     plt.tight_layout()
#     plt.savefig(kwargs['filepath'])
#
#
# def draw_binary_plots(df_list, title, **kwargs):
#     dc = DiscreteCounter(df_list[0].association_counts,
#                          df_list[1].association_counts,
#                          df_list[2].association_counts)
#     dc.make_table()
#     df = dc.data_frame
#     pdc_col = df[['Canopus Count', 'Vega Count']].sum(axis=1)
#     df['Public Display Count'] = pdc_col
#     ax = df.plot(x='Person ID',
#                  y=['Personal Device Count', 'Public Display Count'],
#                  kind='bar', title=title, figsize=(8, 6))
#     ax.set_ylabel('Counts')
#     plt.tight_layout()
#     plt.savefig(kwargs['filepath'])


def non_zero(data, name):
    return data[data[name] != 0][name]


def make_plots(data, prefix, outdir, figsize):
    df = data.time_slices_table[['Personal Device', 'Public Device']]
    df.to_csv(outdir / '{}_time_slices.csv'.format(prefix))

    personal_nz = non_zero(df, 'Personal Device')
    public_nz = non_zero(df, 'Public Device')

    fig, ax = plt.subplots(nrows=2, ncols=2, figsize=figsize)
    sns.distplot(personal_nz, ax=ax[0, 0])
    sns.distplot(public_nz, ax=ax[0, 1])
    qqplot(personal_nz, line='s', markersize=3, ax=ax[1, 0])
    qqplot(public_nz, line='s', markersize=3, ax=ax[1, 1])
    plt.title('{} Screen'.format(prefix.capitalize()), x=0.5, y=0)
    plt.savefig(outdir / '{}_hist_kde_qqplot.png'.format(prefix))


def do_analysis(single, multi, options):
    dir_name = datetime.now().strftime('%Y%m%d-%H%M%S')
    if options.title:
        dir_name += '-{}'.format(options.title)

    figsize = (options.fig_width, options.fig_height)

    outdir = Path(options.outdir) / dir_name
    outdir.mkdir(parents=True, exist_ok=True)

    make_plots(single, 'single', outdir, figsize)
    make_plots(multi, 'multi', outdir, figsize)

    make_multi_scatter('canopus', 'vega', outdir=outdir, df_list=multi.separate)
    make_multi_scatter('canopus', 'pd', df_list=multi.separate, outdir=outdir,
                       title='Personal Device vs Canopus')
    make_multi_scatter('vega', 'pd', df_list=multi.separate, outdir=outdir,
                       title='Personal Device vs Vega')

    def report_tests(name):
        x, x_normal = test_nomality(multi.time_slices_table,
                                    name, options.alpha, 'Multi-screen')
        y, y_normal = test_nomality(single.time_slices_table, name,
                                    options.alpha, 'Single-screen')

        if x_normal and y_normal:
            t_stat, t_pvalue = ttest_ind(x, y)
            logger.info('{name}: t-test={stat:.4f}, pvalue={pvalue:.4f}'.format(
                name=name, stat=t_stat, pvalue=t_pvalue))
        else:
            u_stat, u_pvalue = mannwhitneyu(x, y)
            logger.info('{name}: Mann-Whitney statistic={stat:.4f}, '
                        'pvalue={pvalue:.4f}'.format(name=name, stat=u_stat,
                                                     pvalue=u_pvalue))
            rs_stat, rs_pvalue = ranksums(x, y)
            logger.info('{name}: Wilcoxon rank-sum statistic={stat:.4f}, '
                        'pvalue={pvalue:.4f}'.format(name=name, stat=rs_stat,
                                                     pvalue=rs_pvalue))

    report_tests('Personal Device')
    report_tests('Public Device')


def main():
    options = parse_options()
    set_log_level(logger, options.log_level)

    multi_df = pd.read_csv(options.multi_file)
    multi = make_combined_data(multi_df, options)
    single_df = pd.read_csv(options.single_file)
    single = make_combined_data(single_df, options, use_vega=False)

    do_analysis(single, multi, options)


if __name__ == '__main__':
    main()
