#!/usr/bin/env python

import pandas as pd
import sys

from argparse import ArgumentParser


def parse_options():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('--input', action='append')
    parser.add_argument('--output-file')

    return parser.parse_args()


def main():
    options = parse_options()

    df = pd.DataFrame()
    for f_path in options.input:
        df = df.append(pd.read_csv(f_path))

    df.to_csv(options.output_file, index=False)


if __name__ == '__main__':
    main()
