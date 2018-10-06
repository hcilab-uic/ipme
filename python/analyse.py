#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import numpy as np
import sys

from argparse import ArgumentParser


def parse_options():
    parser = ArgumentParser(sys.argv[0])
    parser.add_argument('--input')
    
    return parser.parse_args()
    
    
def main():
    pass


if __name__ == '__main__':
    main()