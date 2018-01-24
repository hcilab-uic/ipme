#!/usr/bin/env python3


import os
import sys
import csv

from argparse import ArgumentParser


milliseconds_per_frame = 1000 / 29.97

id_dict = {
    '1001': 0,
    '1002': 1,
    '1003': 2,
    '1004': 3,
    '1005': 4,
    '1006': 5,
    '1007': 6,
    '1008': 7,
    '1009': 8,
    '1020': 9,
    '1011': 10,
    '1012': 11,
    '1013': 12,
    '1014': 13,
    '1015': 14
}


def parse_config():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('-i', '--input-file')
    parser.add_argument('-o', '--output-file')

    return parser.parse_args()


def make_frame(data_points):
    first_timestamp = None
    row = [0.0] * 7 * 5 * 3
    for point in data_points:
        if not first_timestamp:
            first_timestamp = point[0]
        elif point[0] < first_timestamp:
            first_timestamp = point[0]
        try:
            idx = id_dict[point[1]] * 7
            for cell in point[6:]:
                row[idx] = float(cell)
                idx += 1
        except KeyError:
            print('keyerror {}'.format(point[1]))
    return [first_timestamp] + row


def process(ifile_name, ofile_name):
    start_timestamp = None    
    last_timestamp = 0
    frames = []
    frame_index = 0
    with open(ifile_name) as f:
        line_no = 0
        frame_entries = []
        for line in f:
            line_no += 1
            if line_no == 1:
                continue
            
            cells = line.rstrip().split(',')
            timestamp = int(cells[0])
            if not start_timestamp:
                start_timestamp = timestamp

            frame_entries.append(cells)
            
            if (timestamp - last_timestamp) >= milliseconds_per_frame:
                last_timestamp = timestamp
                frame_index += 1
                current_frame = make_frame(frame_entries)
                frame_entries = []                
                seconds = (int(current_frame[0]) - start_timestamp) / 1000
                # frames.append([frame_index, seconds] + current_frame)
                frames.append(current_frame)
                
    with open(ofile_name, 'w') as f:
        writer = csv.writer(f, delimiter=',')
        writer.writerows(frames)


def main():
    options = parse_config()
    process(options.input_file, options.output_file)


if __name__ == '__main__':
    main()
