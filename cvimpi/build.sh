#!/bin/bash

cd ./build
rm -r -f *
rm -f *
cmake ..
make 
#make install