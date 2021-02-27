#!/bin/bash

### NOISY ###

output_root=/home/data/DS20924/data
outpath_MEMS=/home/data/DS20924/data/MEMS
outpath_DS20924=/home/data/DS20924/data/DS20924
outpath_DS20924FW=/home/data/DS20924/data/DS20924FW

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")


# BUS
#for idx in 2 3 1 0 ; do
#  for i in /home/data/CHiME4_bth_5ch/dt*${arr_dir[$idx]}_simu; do
#    #echo $i
#    dir="$(echo $i | cut -d'/' -f5)"
#    #echo $dir
#    for j in $(ls $i); do
#      input=$i'/'$j
#      mkdir -p $outpath_MEMS'/'$dir
#      mkdir -p $outpath_DS20924'/'$dir
#      mkdir -p $outpath_DS20924FW'/'$dir
#
#      echo ./DS20924.out $output_root $dir'/'$j MEMS DS20924 DS20924FW  $input /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav
#      ./DS20924.out $output_root $dir'/'$j MEMS DS20924 DS20924FW  $input /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav
#
#    done
#    sleep 600
#  done
#done
#
#echo '**************************************************************'
#sleep 3600
#

for idx in 2 3 1 0 ; do
  for i in /home/data/CHiME4_bth_5ch/et*${arr_dir[$idx]}_simu; do
    #echo $i
    dir="$(echo $i | cut -d'/' -f5)"
    #echo $dir
    for j in $(ls $i); do
      input=$i'/'$j
      mkdir -p $outpath_MEMS'/'$dir
      mkdir -p $outpath_DS20924'/'$dir
      mkdir -p $outpath_DS20924FW'/'$dir

      echo ./DS20924.out $output_root $dir'/'$j MEMS DS20924 DS20924FW  $input /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav
      ./DS20924.out $output_root $dir'/'$j MEMS DS20924 DS20924FW  $input /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav

    done
    sleep 600
  done
done
