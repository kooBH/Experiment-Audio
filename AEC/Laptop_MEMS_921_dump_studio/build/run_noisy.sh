#!/bin/bash

### NOISY ###

outpath=/home/mini/data/recording/20210916_AEC_MEMS_921/
#wav_root=/home/data/DS20921/data/
wav_root=/home/mini/data/AEC/longer_noise/

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")

TARGET=Noise1

# AEC <clean> <noise> <path1> <path2> <path3> <category> <name>
#  0     1       2        3      4        5       6        7

#while :
#do

# for category
#for idx in 0 1 2 3 ; do
#for idx in 2 3 ; do

#while :
#do

#sleep 30

for idx in 0 1 2 3 ; do
  # for  et,dt 
  for i in /home/mini/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
  #for i in /home/data/CHiME4_bth_5ch/dt05_${arr_dir[$idx]}_simu; do
    dir="$(echo $i | cut -d'/' -f6)"
    mkdir -p $outpath'/'MEMS'/'$dir
    mkdir -p $outpath'/'DS20921'/'$dir
    mkdir -p $outpath'/'reference'/'$dir
    mkdir -p $outpath'/'dump'/'$dir

# for files 
     for j in $(ls $i); do
      echo ./RECORD $wav_root'/'Clean'/'$j  $wav_root'/'${TARGET}'/'$j   $dir $j $outpath'/'MEMS $outpath'/'DS20921
		./RECORD $wav_root'/'Clean'/'$j  $wav_root'/'${TARGET}'/'$j   $dir $j $outpath'/'MEMS $outpath'/'DS20921 $outpath'/'dump
		cp $wav_root'/'${TARGET}'/'$j $outpath'/'reference'/'$dir'/'$j
#    break
	 done


	 #Residual fix
	#for j in $i/M04*; do
	#	fn="$(echo $j | cut -d'/' -f6)"
#		./RECORD $wav_root'/'Clean'/'$fn  $wav_root'/'${TARGET}'/'$fn   $dir $fn $outpath'/'MEMS $outpath'/'DS20921
#		cp $wav_root'/'${TARGET}'/'$fn $outpath'/'reference'/'$dir'/'$fn
#	done
#break
  done
 #break
done

#done
