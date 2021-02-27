#!/bin/bash

### NOISY ###

outpath_root=/home/data/20210205
outpath_device_1=/home/data/20210205/DMEMS
outpath_device_2=/home/data/20210205/KETI_L
outpath_device_3=/home/data/20210205/KETI_R
outpath_device_4=/home/data/20210205/DS20924FW

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
      mkdir -p $outpath_device_4'/'$dir
	  echo ./20210205 $outpath_root $dir'/'$j KETI_R DMEMS KETI_L DS20924FW $input
	  ./20210205 $outpath_root $dir'/'$j KETI_R DMEMS KETI_L DS20924FW $input
    done
  done
done

