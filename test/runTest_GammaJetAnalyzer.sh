#!/bin/sh

progName="test_GammaJetAnalyzer";
###
inputFile="/mnt/hadoop/cms/store/user/luck/2014-photon-forests/pp_Data_localJEC_v3_HiForest_nodupes.root";
colType=0;
###
#inputFile="/mnt/hadoop/cms/store/user/luck/2014-photon-forests/pPb_DATA_photon30trig_localJEC_v2.root";
#colType=1;
###
#inputFile="/mnt/hadoop/cms/store/user/tatar/2015-photon-forests/pPb_Pbp_MIX_localJEC_v3_pthatsorted/merged_pPb_Pbp_MIX_AllQCDPhoton30_50_80_120_170_localJEC_v3_ptHat_30_50.root";
#colType=1;
###

outputFile="test_GammaJetAnalyzer_v0907.root";

g++ $progName.C $(root-config --cflags --libs) -Wall -O2 -o $progName.exe

./$progName.exe $inputFile $outputFile $colType
