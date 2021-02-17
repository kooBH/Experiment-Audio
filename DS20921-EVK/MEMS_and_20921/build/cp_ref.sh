#!/bin/bash

### NOISY ###

wav_root=/home/data/DS20921/

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")

# BUS
for idx in 0 1 2 3 ; do
  for i in /home/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
    #echo $i
    dir="$(echo $i | cut -d'/' -f5)"
    #echo $dir
    for j in $(ls $i); do
      mkdir -p $wav_root'/'noise_1'/'$dir
      mkdir -p $wav_root'/'noise_2'/'$dir
      mkdir -p $wav_root'/'noise_12'/'$dir
      cp $wav_root'/'data'/'Noise1'/'$j $wav_root'/'noise_1'/'$dir'/'$j
      cp $wav_root'/'data'/'Noise2'/'$j $wav_root'/'noise_2'/'$dir'/'$j
      cp $wav_root'/'data'/'Noise12'/'$j $wav_root'/'noise_12'/'$dir'/'$j
    done
  done
done
