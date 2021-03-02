#!/bin/bash

### NOISY ###

outpath=/home/data/AEC/20210228_AEC/
wav_root=/home/data/AEC/

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")

TARGET=Noise1

# AEC <clean> <noise> <path1> <path2> <path3> <category> <name>
#  0     1       2        3      4        5       6        7

#while :
#do
for idx in 0 1 2 3 ; do
  for i in /home/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
    dir="$(echo $i | cut -d'/' -f5)"
    for j in $(ls $i); do
      mkdir -p $outpath'/'MEMS'/'$dir
      mkdir -p $outpath'/'DS20921'/'$dir
      mkdir -p $outpath'/'DS20924'/'$dir
      echo ./RECORD $wav_root'/'Clean'/'$j  $wav_root'/'${TARGET}'/'$j  $outpath'/'MEMS $outpath'/'DS20921 $outpath'/'DS20924 $dir $j
      ./RECORD $wav_root'/'Clean'/'$j  $wav_root'/'${TARGET}'/'$j  $outpath'/'MEMS $outpath'/'DS20921 $outpath'/'DS20924 $dir $j
	 # break
    done
#	break
  done
done

#done
