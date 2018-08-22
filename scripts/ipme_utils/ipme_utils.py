import csv


def write_csv(rows, file_path):
    with open(file_path, 'w') as f:
        writer = csv.writer(f)
        writer.writerows(rows)
