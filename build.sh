#!/bin/bash

mkdir Build-Scratch
pushd Build-Scratch
cmake ../
make -j 8
popd
