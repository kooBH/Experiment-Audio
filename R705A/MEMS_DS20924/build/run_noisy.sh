#!/bin/bash

### NOISY ###

output_root=/home/data/R705A
outpath_MEMS=/home/data/R705A/MEMS
outpath_DS20924=/home/data/R705A/DS20924

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")

#sleep 7200

sleep 30

# BUS
for idx in 0 1 2 3 ; do
  for i in /home/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
    #echo $i
    dir="$(echo $i | cut -d'/' -f5)"
    #echo $dir
    for j in $(ls $i); do
      input=$i'/'$j
      mkdir -p $outpath_MEMS'/'$dir
      mkdir -p $outpath_DS20924'/'$dir

      echo ./RECORD $output_root $dir'/'$j MEMS DS20924 $input /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav
      ./RECORD $output_root $dir'/'$j MEMS DS20924 $input /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav
    done
 #   sleep 600
  done
done

