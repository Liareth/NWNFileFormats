#!/bin/bash

mkdir Build-Scratch
pushd Build-Scratch
cmake ../
make
popd
