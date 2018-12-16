#!/usr/bin/env python

import numpy as np
import pandas as pd
import sys

from argparse import ArgumentParser
from pathlib import Path


def parse_options():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('--input-dir', required=True)
    parser.add_argument('--input-pattern', default='**/study_*-person-*.csv')
    parser.add_argument('--output-file', required=True)
    return parser.parse_args()


def main():
    options = parse_options()
    input_dir = Path(options.input_dir)
    table = []
    for file_path in input_dir.glob(options.input_pattern):
        df = pd.read_csv(file_path)
        table.append([
            file_path.stem,
            np.average(df['Personal Device']),
            np.average(df['Public Device'])])

    out_df = pd.DataFrame(data=table, columns=['key', 'Personal', 'Public'])
    out_df.to_csv(options.output_file)


if __name__ == '__main__':
    main()
