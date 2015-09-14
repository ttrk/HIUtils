#!/bin/sh

progName="fixPythonImport_v2";
input="/home/kaya/Documents/cmsswHI";

g++ $progName.C $(root-config --cflags --libs) -Wall -O2 -o $progName.exe || exit 1

./$progName.exe $input || exit 1
