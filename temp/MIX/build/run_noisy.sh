#!/bin/bash

### NOISY ###

outpath=/home/data/DS20921/MIX/
wav_root=/home/data/DS20921/data/

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")


# BUS
for idx in 0 1 2 3 ; do
  for i in /home/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
    #echo $i
    dir="$(echo $i | cut -d'/' -f5)"
    #echo $dir
    for j in $(ls $i); do
      mkdir -p $outpath'/AKG/'$dir
      mkdir -p $outpath'/MEMS/'$dir
      echo ./DS20921 $wav_root'/'Clean'/'$j $wav_root'/'Noise1'/'$j  $outpath $dir'/'$j
      ./DS20921 $wav_root'/'Clean'/'$j $wav_root'/'Noise1'/'$j  $outpath $dir'/'$j
    done
  done
done
