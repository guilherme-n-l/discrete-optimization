#!/usr/bin/python
# -*- coding: utf-8 -*-
from subprocess import run


def solve_it(input_data):
    exec = run(["./main", input_data], capture_output=True)
    return exec.stdout.decode('UTF-8')


if __name__ == "__main__":
    print("This script submits the integer: %s\n" % solve_it(""))
