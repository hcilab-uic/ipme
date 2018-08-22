#!/usr/bin/env python3

import logging
import math
import sys

import numpy as np
import quaternion

from argparse import ArgumentParser

from analysis import experiment


FORMAT = '%(asctime)-15s %(levelname)s %(filename)-8s %(message)s'
logging.basicConfig(format=FORMAT)
logger = logging.getLogger('main')
logger.setLevel(logging.DEBUG)


def parse_args():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('-i', '--input-path', required=True)
    parser.add_argument('-o', '--output-path', required=True)

    return parser.parse_args()


class Analysis(object):
    def __init__(self, **kwargs):
        self.__computed_frames = kwargs['computed_frames']
        self.__compute_frames()

    def __compute_frames(self):
        for frame in self.__computed_frames:
            Analysis.compute_affinities(frame)

    @staticmethod
    def compute_affinities(frame):
        for person in frame['persons']:
            for device in frame['devices']:
                q1 = person['rotation']
                q2 = device['rotation']
                logger.info('angle {}'.format((q1 * q2).norm()))


def main():
    options = parse_args()
    exp = experiment.load_experiment(options.input_path)
    computed_frames = exp.computed_frames
    logger.info('Number of computed frames {}'.format(len(computed_frames)))
    analysis = Analysis(computed_frames=computed_frames)


if __name__ == '__main__':
    main()