#!/usr/bin/env python3

import csv
import sys
from argparse import ArgumentParser


# these hacks need to go away
fields_per_point = 7
id_offset = 1001


def read_config():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('-f', '--file')
    parser.add_argument('-p', '--points-in-frame', default=20)
    parser.add_argument('-o', '--out-file', default='output.csv')
    parser.add_argument('-d', '--dense', action='store_true', default=False)
    return parser.parse_args()


def process(filename, points_in_frame, dense):
    number_of_fields = (fields_per_point * points_in_frame)
    csv_rows = []
    with open(filename) as f:
        reader = csv.reader(f)
        timestamp = 0
        frame_ids = set()
        expanded_row = ['0.0'] * number_of_fields
        for i, row in enumerate(reader):
            if i == 0:
                continue
            timestamp = max(timestamp, int(row[0]))
            device_id = int(row[1])
            start_column = ((device_id - id_offset) * fields_per_point)

            frame_point = row[-7:]
            for field_index, field in enumerate(frame_point):
                try:
                    expanded_row[start_column + field_index] = str(field)
                except IndexError:
                    print('length {}, index {}, start {}, f_index {}'.format(
                        len(expanded_row), start_column + field_index,
                        start_column, field_index))

            if dense:
                frame_ids.add(device_id)
                if len(frame_ids) >= points_in_frame:
                    csv_rows.append([str(timestamp)] + expanded_row)
                    expanded_row = ['0.0'] * number_of_fields
                    frame_ids.clear()
            else:
                if device_id in frame_ids:
                    csv_rows.append([str(timestamp)] + expanded_row)
                    expanded_row = ['0.0'] * number_of_fields
                    frame_ids.clear()
                else:
                    frame_ids.add(device_id)
    return csv_rows


def write_to_file(csv_rows, filename):
    with open(filename, 'w') as csvfile:
        writer = csv.writer(csvfile)
        for row in csv_rows:
            s_row = [str(f) for f in row]
            writer.writerow(s_row)


def main():
    options = read_config()
    csv_rows = process(options.file, int(options.points_in_frame),
                       options.dense)
    write_to_file(csv_rows, options.out_file)


if __name__ == '__main__':
    main()