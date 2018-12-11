#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
import sys

from argparse import ArgumentParser
from datetime import datetime
from pathlib import Path
from scipy.stats import mannwhitneyu, shapiro, ttest_ind, ranksums, \
    ttest_rel, wilcoxon
from statsmodels.graphics.gofplots import qqplot


from utils.log import setup_logger, set_log_level


logger = setup_logger(__name__)


def parse_options():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('--single-file')
    parser.add_argument('--multi-file')
    parser.add_argument('--outdir', default=str(Path()))
    parser.add_argument('--log-level', default='info')
    parser.add_argument('--title')
    parser.add_argument('--fig-width', default=10)
    parser.add_argument('--fig-height', default=8)
    parser.add_argument('--alpha', default=0.01)
    parser.add_argument('--transform-type', default='10',
                        choices=['none', '2', 'natural', '10'])
    parser.add_argument('--remove-outliers', action='store_true', default=False)

    return parser.parse_args()


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


def test_nomality(data, name, alpha, msg_tag):
    values = data[name]
    logger.debug('Performing Shapiro-Wilk test on {name}({tag}), '
                 'length={len}'.format(name=name, len=len(values),
                                       tag=msg_tag))
    w, pvalue = shapiro(values)
    normal = False if pvalue < alpha else True
    logger.info('Normality test for {name}({tag}), W={w:.4f}, '
                'pvalue={pvalue:.4f}, normal={normal}'.format(
        w=w, pvalue=pvalue, normal=normal, tag=msg_tag, name=name))
    return values, normal


def make_plots(data, prefix, outdir, figsize):
    # df = data[['Personal Device', 'Public Device']]
    # df.to_csv(outdir / '{}_time_slices.csv'.format(prefix))
    #
    x = data['Personal Device']
    y = data['Public Device']

    try:
        fig, ax = plt.subplots(nrows=2, ncols=2, figsize=figsize)
        sns.distplot(x, ax=ax[0, 0])
        sns.distplot(y, ax=ax[0, 1])
        qqplot(x, line='s', markersize=3, ax=ax[1, 0])
        qqplot(y, line='s', markersize=3, ax=ax[1, 1])
        plt.title('{} Screen'.format(prefix.capitalize()), x=-0.25, y=2.3)
        plt.savefig(outdir / '{}_hist_kde_qqplot.png'.format(prefix))
    except ValueError as err:
        logger.error(err)


def report_tests(single_ts, multi_ts, name, options, msg_tag):
    y, y_normal = test_nomality(single_ts, name, options.alpha, 'Single-screen')
    x, x_normal = test_nomality(multi_ts, name, options.alpha, 'Multi-screen')

    table = []
    def print_report(result, test_name):
        outcome = 'REJECT' if result[1] < options.alpha else 'Do Not REJECT'
        logger.info('{name}: {test} stat={stat:.4f}, '
                    'pvalue={pvalue:.4f}, {outcome}'.format(
            name=msg_tag, stat=result[0], pvalue=result[1], test=test_name,
            outcome=outcome))
        table.append([msg_tag, test_name, result[0], result[1], outcome])

    if x_normal and y_normal:
        print_report(ttest_ind(x, y), 'T-Test')
    else:
        print_report(mannwhitneyu(x, y), 'Mann-Whitney U')
        print_report(ranksums(x, y), 'Wilcoxon Rank-Sum')
        print_report(wilcoxon(x, y), 'Wilcoxon Signed-Rank')

    headers = ['Type', 'Test', 'Stat', 'p-value', 'Null Hypothesis']
    return pd.DataFrame(data=table, columns=headers)


def make_reports(single_ts, multi_ts, outdir, options, tag=''):
    figsize = (options.fig_width, options.fig_height)

    make_plots(single_ts, 'single', outdir, figsize)
    single_ts.to_csv(outdir / 'single.csv', index=True)
    make_plots(multi_ts, 'multi', outdir, figsize)
    multi_ts.to_csv(outdir / 'multi.csv', index=True)

    df = pd.DataFrame()
    for device in ['personal', 'public']:
        device_name = '{} Device'.format(device.capitalize())
        msg_tag = '{device_name} ({tag})'.format(device_name=device_name,
                                                 tag=tag)
        try:
            df_report = report_tests(single_ts,
                                     multi_ts, device_name, options,
                                     msg_tag)
            df = df.append(df_report)
        except ValueError as err:
            logger.error('Report generation failed for {} [{}]'.format(
                device, err))
    df.to_csv(outdir / '{}.csv'.format(tag))


def main():
    options = parse_options()
    set_log_level(logger, options.log_level)

    dir_name = datetime.now().strftime('%Y%m%d-%H%M%S')
    if options.title:
        dir_name += '-{}'.format(options.title)

    outdir = Path(options.outdir) / dir_name
    outdir.mkdir(parents=True, exist_ok=True)

    single_df = pd.read_csv(options.single_file)
    multi_df = pd.read_csv(options.multi_file)

    if options.transform_type != 'none':
        single_table = []
        for row in single_df.values:
            single_table.append([compute_transform(v, options.transform_type)
                                 for v in row])

        single_df = pd.DataFrame(data=single_table,
                                 columns=single_df.columns.tolist())

        multi_table = []
        for row in multi_df.values:
            multi_table.append([compute_transform(v, options.transform_type)
                                 for v in row])

        multi_df = pd.DataFrame(data=multi_table,
                                 columns=multi_df.columns.tolist())

    make_reports(single_df, multi_df,
                 outdir, options, tag='Personal Device')


if __name__ == '__main__':
    main()
