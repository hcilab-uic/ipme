#!/usr/bin/env python3

import csv
import matplotlib as plt
import numpy as np
import re
import seaborn as sns
import sys

from argparse import ArgumentParser
from scipy.stats import mannwhitneyu, shapiro, ttest_rel, ttest_ind, \
    ranksums, wilcoxon

pattern = re.compile('(?P<first>[a-z0-9]{2})-.*-(?P<second>p[a-z0-9]*)')

from utils.log import setup_logger, set_log_level

logger = setup_logger(__name__)


def parse_options():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('--input-file')
    parser.add_argument('--log-level', default='info')

    return parser.parse_args()


def extract(row):
    return np.array([float(row['study-1']), float(row['study-2']),
                     float(row['study-3'])])


def read_data(file_path):
    d = {}
    with open(file_path) as f:
        reader = csv.DictReader(f)
        prev = {}
        for i, row in enumerate(reader):
            bits = row['type'].split('-')
            if i % 2 == 0:
                prev = {bits[1]: extract(row)}
            else:
                current = {bits[1]: extract(row)}
                key = '{}-{}'.format(bits[0], bits[2])
                d.update({key: {}})
                d[key].update(prev)
                d[key].update(current)
                prev = {}
    return d


def test_normality(values):
    logger.debug('running shapiro on {}'.format(values))
    w, pvalue = shapiro(values)
    normal = False if pvalue < 0.01 else True
    logger.debug('Normal={}'.format(normal))
    return values, normal


def analyze(key, values):
    single, single_normal = test_normality(values['single'])
    multi, multi_normal = test_normality(values['multi'])

    def print_report(result, test_name):
        outcome = 'REJECT' if result[1] < 0.01 else 'Do Not REJECT'
        logger.info('{key} {test} stat={stat:.4f}, '
                    'pvalue={pvalue:.4f}, {outcome}'.format(
            stat=result[0], pvalue=result[1], test=test_name,
            outcome=outcome, key=key))

    if single_normal and multi_normal:
        print_report(ttest_rel(single, multi), 'T-Test')
    else:
        print_report(wilcoxon(single, multi), 'Wilcoxon-Signed Rank')


def main():
    options = parse_options()
    set_log_level(logger, options.log_level)

    data = read_data(options.input_file)
    for k, v in data.items():
        analyze(k, v)


if __name__ == '__main__':
    main()
