#!/bin/bash

### NOISY ###

output_root=/home/data/DS20924/
outpath_MEMS=/home/data/DS20924/MEMS
outpath_DS20924=/home/data/DS20924/DS20924
outpath_DS20924FW=/home/data/DS20924/DS20924FW

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")


# BUS
#while :
#do

for idx in 2 0 1 3; do
  for i in /home/data/CHiME4_bth_5ch/*${arr_dir[$idx]}_simu; do
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
	break
    done
	break
  done
 # break
done

#done
