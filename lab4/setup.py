#! /usr/bin/env python3

import sys
from os import path

from setuptools import setup, find_packages

BASE_PATH = path.dirname(path.abspath(__file__))

with open(f"{BASE_PATH}/requirements.txt", "r") as fp:
    requirements = fp.read().splitlines()


setup(
  name="delta-debugger",
  version="1.0.0",    python_requires=">=3.6",
    description="A simple character-wise delta debugger.",
    entry_points={"console_scripts": ["delta-debugger=delta_debugger.__main__:main"]},
    packages=find_packages(include=["delta_debugger", "delta_debugger.*"]),
    install_requires=requirements,
)
