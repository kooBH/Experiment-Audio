#!/bin/bash

### NOISY ###

outpath=/home/data/AEC/20210810_AEC_MEMS_921/
#wav_root=/home/data/DS20921/data/
wav_root=/media/kiosk/IIP/Experiment-Audio/

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")

TARGET=Noise1

# AEC <clean> <noise> <path1> <path2> <path3> <category> <name>
#  0     1       2        3      4        5       6        7

#while :
#do

# for category
for idx in 0 1 2 3 ; do
  # for  et,dt 
  for i in /home/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
    dir="$(echo $i | cut -d'/' -f5)"
	# for files 
    for j in $(ls $i); do
      mkdir -p $outpath'/'MEMS'/'$dir
      mkdir -p $outpath'/'DS20921'/'$dir
      mkdir -p $outpath'/'reference'/'$dir
      echo ./RECORD $wav_root'/'Clean'/'$j  $wav_root'/'${TARGET}'/'$j   $dir $j $outpath'/'MEMS $outpath'/'DS20921
      ./RECORD $wav_root'/'Clean'/'$j  $wav_root'/'${TARGET}'/'$j   $dir $j $outpath'/'MEMS $outpath'/'DS20921
	  cp $wav_root'/'${TARGET}'/'$j $outpath'/'reference'/'$dir'/'$j
	  break
    done
  done
done

#done
