#!/bin/sh

progName="fixPythonImport";
input="/home/kaya/Documents/cmssw";

g++ $progName.C $(root-config --cflags --libs) -Wall -O2 -o $progName.exe || exit 1

./$progName.exe $input || exit 1
