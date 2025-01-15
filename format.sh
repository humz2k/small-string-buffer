#!/usr/bin/env bash

clang-format -i include/smallstring/*.hpp
clang-format -i example.cpp
clang-format -i benchmark.cpp