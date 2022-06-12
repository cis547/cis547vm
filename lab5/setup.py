#! /usr/bin/env python3

import sys
from os import path

from setuptools import setup, find_packages

BASE_PATH = path.dirname(path.abspath(__file__))

with open(f"{BASE_PATH}/requirements.txt", "r") as fp:
    requirements = fp.read().splitlines()

if sys.version_info < (3, 7):
    requirements.append("dataclasses")

setup(
    name="cbi",
    python_requires=">=3.6",
    description="Tool for cooperative bug isolation",
    entry_points={"console_scripts": ["cbi=cbi.__main__:main"]},
    packages=find_packages(include=["cbi", "cbi.*"]),
    install_requires=requirements,
)
