#!/bin/bash

### NOISY ###

outpath=/home/data/DS20921/MEMS_20921
wav_root=/home/data/DS20921/data/

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")

NOISE=Noise1

#for idx in 0 1 2 3 ; do
#  for i in /home/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
#    #echo $i
#    dir="$(echo $i | cut -d'/' -f5)"
#    #echo $dir
#    for j in $(ls $i); do
#      mkdir -p $outpath'/MEMS/'$dir
#      mkdir -p $outpath'/DS20921/'$dir
#      echo ./DS20921.out $wav_root'/'Clean'/'$j  $wav_root'/'${NOISE}'/'$j   $outpath'/MEMS/'$dir'/'$j $outpath'/DS20921/'$dir'/'$j
#      ./DS20921.out $wav_root'/'Clean'/'$j  $wav_root'/'${NOISE}'/'$j   $outpath'/MEMS/'$dir'/'$j $outpath'/DS20921/'$dir'/'$j
#    done
#  done
#done

for idx in 3 ; do
  for i in /home/data/CHiME4_bth_5ch/et*${arr_dir[$idx]}_simu; do
    #echo $i
    dir="$(echo $i | cut -d'/' -f5)"
    #echo $dir
    for j in $(ls $i); do
      mkdir -p $outpath'/MEMS/'$dir
      mkdir -p $outpath'/DS20921/'$dir
      echo ./DS20921.out $wav_root'/'Clean'/'$j  $wav_root'/'${NOISE}'/'$j   $outpath'/MEMS/'$dir'/'$j $outpath'/DS20921/'$dir'/'$j
      ./DS20921.out $wav_root'/'Clean'/'$j  $wav_root'/'${NOISE}'/'$j   $outpath'/MEMS/'$dir'/'$j $outpath'/DS20921/'$dir'/'$j
    done
  done
done

