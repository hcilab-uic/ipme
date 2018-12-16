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
    parser.add_argument('--input-file', action='append')
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


def remove_outlier_data(data, m=2):
    return data[abs(data - np.mean(data)) < m * np.std(data)]


def test_nomality(all_values, name, alpha, msg_tag, remove_outliers):
    # all_values = data[name]
    values = all_values
    if remove_outliers:
        values = remove_outlier_data(all_values, 1)
        logger.info('outlier removel {}->{}'.format(len(all_values), len(values)))

    logger.debug('Performing Shapiro-Wilk test on {name}({tag}), '
                 'length={len}'.format(name=name, len=len(values),
                                       tag=msg_tag))
    w, pvalue = shapiro(values)
    normal = False if pvalue < alpha else True
    logger.info('Normality test for {name}({tag}), W={w:.4f}, '
                'pvalue={pvalue:.4f}, normal={normal}'.format(
        w=w, pvalue=pvalue, normal=normal, tag=msg_tag, name=name))
    return values, normal


def main():
    options = parse_options()
    set_log_level(logger, options.log_level)

    dir_name = datetime.now().strftime('%Y%m%d-%H%M%S')
    if options.title:
        dir_name += '-{}'.format(options.title)

    outdir = Path(options.outdir) / dir_name
    outdir.mkdir(parents=True, exist_ok=True)

    d = {'Personal Device': [], 'Public Device': []}
    for input_file in options.input_file:
        df = pd.read_csv(input_file)
        for k, _ in d.items():
            d[k].append(df[k])

    for k, v in d.items():
        print(k)
        for i, values in enumerate(v):
            test_nomality(values.values, k, options.alpha,
                          'message', options.remove_outliers)


if __name__ == '__main__':
    main()