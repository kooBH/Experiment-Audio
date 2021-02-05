#!/bin/bash

### NOISY ###

outpath_root=/home/data/20210204
outpath_device_1=/home/data/20210204/AKG
outpath_device_2=/home/data/20210204/KETI
outpath_device_3=/home/data/20210204/MEMS

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
      mkdir -p $outpath_device_1'/'$dir
      mkdir -p $outpath_device_2'/'$dir
      mkdir -p $outpath_device_3'/'$dir
      echo ./20210204 $outpath_root $dir'/'$j AKG KETI MEMS $input
      ./20210204 $outpath_root $dir'/'$j AKG KETI MEMS $input
    done
  done
done

