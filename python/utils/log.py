import logging
import sys

from datetime import datetime
from pathlib import Path


FORMAT = '%(asctime)-15s %(levelname)s %(funcName)s:%(lineno)s %(message)s'
log_filename = '{}.log'.format(datetime.now().strftime('%Y%m%d-%H%M%S'))


def setup_logger(name, outdir=Path()):
    logger = logging.getLogger(name)
    formatter = logging.Formatter(FORMAT)

    log_dir = outdir / 'log'
    log_dir.mkdir(exist_ok=True, parents=True)
    log_file_path = log_dir / log_filename

    fh = logging.FileHandler(log_file_path)
    fh.setFormatter(formatter)
    logger.addHandler(fh)

    sh = logging.StreamHandler(sys.stdout)
    sh.setFormatter(formatter)
    logger.addHandler(sh)

    return logger


def set_log_level(logger, level):
    logger.setLevel(logging.getLevelName(level.upper()))
