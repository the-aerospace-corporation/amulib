#!/usr/bin/env bash
# Build the AMU docs: pull the theme submodule (first run) then run Doxygen.
# Works from any directory.
set -e

cd "$(dirname "$0")"                        # docs/ — where the Doxyfile lives
git submodule update --init --recursive    # no-op once the theme is present
doxygen Doxyfile                           # output -> docs/html/index.html

echo "Docs built -> $(pwd)/html/index.html"
