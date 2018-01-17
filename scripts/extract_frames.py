#!/usr/bin/env python3

import csv
import json
import logging
import os
import sys

from argparse import ArgumentParser

""" example config file:

{
  "input_file": "/home/harish/Downloads/vicon_record_20171011_184844.csv",
  "output_file": "/home/harish/data/ipme/study_1/extracted_out.csv",
  "fields_per_rigid_body": 7,
  "entities": [
    {
      "head": "1001",
      "solar_plexus": "1002",
      "right_arm": "1003",
      "left_arm": "1004",
      "device": "1005"
    },
    {
      "head": "1006",
      "solar_plexus": "1007",
      "right_arm": "1008",
      "left_arm": "1009",
      "device": "1020"
    },
    {
      "head": "1011",
      "solar_plexus": "1012",
      "right_arm": "1013",
      "left_arm": "1014",
      "device": "1015"
    }
  ]
}
"""

logging.basicConfig(format='%(asctime)s %(levelname)s %(message)s',
                    level=logging.DEBUG)


def read_config():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('-c', '--config')

    return parser.parse_args()


def compute_field_count(config):
    return (config['fields_per_rigid_body'] *
            len(config['entities']) *
            len(config['entities'][0]))


def create_mapping(config):
    entities = config['entities']
    mapping = {}

    for i, entity in enumerate(entities):
        j = 0
        for k, v in entity.items():
            mapping.update({v: {'entity_slot': i, 'marker_slot': j}})
            j += 1

    return mapping


def compute_start_slot(config, slot_mapping):
    fields_per_marker = config['fields_per_rigid_body']
    entity_slot = slot_mapping['entity_slot']
    marker_slot = slot_mapping['marker_slot']

    # FIXME: don't compute this over and over
    markers_per_entity = len(config['entities'][0])

    return (entity_slot * fields_per_marker * markers_per_entity) + \
           (fields_per_marker * marker_slot)


def process(config):
    table = []
    mapping = create_mapping(config)
    fields_per_marker = config['fields_per_rigid_body']
    markers_per_entity = len(config['entities'][0])
    key_error_count = 0
    entries_processed = 0

    with open(config['input_file']) as f:
        reader = csv.reader(f)
        next(reader, None) # Skip header
        table_row = [0.0] * compute_field_count(config)
        previous_row = table_row

        entity_marker_count = [0] * len(config['entities'])
        timestamp = None
        max_cell_count = 0
        for i, row in enumerate(reader):
            # FIXME: This logic is a bit flawed. It says, record the longest
            # cell count seen so far. If current row has fewer cells that the
            # maximum seen so far, skip. This works here because all rows
            # have same sell count and lets dynamic setting of cell count
            max_cell_count = max(max_cell_count, len(row))
            if not row or len(row) < max_cell_count:
                continue

            # Record the first timestamp of the frame
            if timestamp is None:
                timestamp = int(row[0])

            slot_mapping = None
            try:
                source_id = row[1]
                slot_mapping = mapping[source_id]
            except KeyError as err:
                logging.warning('Key {} was not configured, skipping'.format(
                    source_id))
                key_error_count += 1
                continue
            except IndexError:
                logging.warning('Key {} is out of range'.format(row))

            entity_marker_count[slot_mapping['entity_slot']] += 1
            data_row = row[6:]
            start_slot = compute_start_slot(config, slot_mapping)
            for cell in range(0, fields_per_marker):
                slot = cell + start_slot
                table_row[slot] = data_row[cell]
            entries_processed += 1

            for entity in entity_marker_count:
                # Record frame even if one entity has all the markers
                if entity == markers_per_entity:
                    # logging.info('entity = {}'.format(entity))
                    entity_marker_count = [0] * len(config['entities'])
                    for col in range(len(table_row)):
                        if table_row[col] == 0.0:
                            table_row[col] = previous_row[col]
                    table.append([timestamp] + table_row)
                    previous_row = table_row
                    table_row = [0.0] * compute_field_count(config)
                    timestamp = None

    return table, entries_processed, key_error_count


def write_to_file(csv_rows, filename):
    parent_dirname = os.path.dirname(os.path.abspath(filename))
    os.makedirs(parent_dirname, exist_ok=True)
    with open(filename, 'w') as csvfile:
        writer = csv.writer(csvfile)
        for row in csv_rows:
            s_row = [str(f) for f in row]
            writer.writerow(s_row)


def main():
    options = read_config()
    with open(options.config) as f:
        config = json.load(f)
        table, count, error_count = process(config)
        write_to_file(table, config['output_file'])
        logging.info('Processing complete')
        logging.info('Output was written to {}'.format(config['output_file']))
        logging.info('Entries processed {}, errors {}, rows {}'.format(
            count, error_count, len(table)))


if __name__ == '__main__':
    main()
