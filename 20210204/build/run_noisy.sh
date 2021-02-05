#!/bin/bash

### NOISY ###

outpath_raw_MLDR=/home/data/noisy/raw_MLDR
outpath_raw_Conex=/home/data/noisy/raw_Conexant
outpath_MLDR=/home/data/noisy/MLDR
outpath_Conexant=/home/data/noisy/Conexant

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")


# BUS
for idx in 0 1 2 3 ; do
  for i in /home/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
    #echo $i
    dir="$(echo $i | cut -d'/' -f5)"
    #echo $dir
    for j in $(ls $i); do
      input=$i'/'$j
      mkdir -p $outpath_raw_MLDR'/'$dir
      mkdir -p $outpath_raw_Conex'/'$dir
      mkdir -p $outpath_MLDR'/'$dir
      mkdir -p $outpath_Conexant'/'$dir
      echo ./Dual $input $outpath_raw_MLDR'/'$dir'/'$j $outpath_MLDR'/'$dir'/'$j $outpath_raw_Conex'/'$dir'/'$j $outpath_Conexant'/'$dir'/'$j /home/data/CHiME4_Noise/longest/${arr_nos[$idx]}.wav
      ./MEMS $input $outpath_raw_MLDR'/'$dir'/'$j $outpath_MLDR'/'$dir'/'$j $outpath_raw_Conex'/'$dir'/'$j $outpath_Conexant'/'$dir'/'$j /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav
    done
    sleep 600
  done
done
