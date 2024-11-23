#!/usr/bin/python
# -*- coding: utf-8 -*-

from subprocess import run


def solve_it(input_data):
    p = run("./main", input=input_data, capture_output=True, text=True)
    return p.stdout


import sys

if __name__ == "__main__":
    import sys

    if len(sys.argv) > 1:
        file_location = sys.argv[1].strip()
        with open(file_location, "r") as input_data_file:
            input_data = input_data_file.read()
        print(solve_it(input_data), end='')
    else:
        print(
            "This test requires an input file.  Please select one from the data directory. (i.e. python solver.py ./data/gc_4_1)"
        )
