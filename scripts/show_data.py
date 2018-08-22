#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys

from argparse import ArgumentParser
from pathlib import Path

from analysis.experiment import Experiment


def parse_args():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('-i', '--input', help='path to input files or dir')
    parser.add_argument('-p', '--print-config', action='store_true',
                        default=False)
    parser.add_argument('-o', '--output', default=Path().absolute())

    return parser.parse_args()


def main():
    options = parse_args()
    experiment = Experiment(options)
    experiment.write()


if __name__ == '__main__':
    main()
