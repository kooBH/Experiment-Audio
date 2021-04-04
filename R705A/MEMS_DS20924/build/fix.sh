#!/bin/bash

### NOISY ###

output_root=/home/data/R705A/
outpath_MEMS=/home/data/R705A/MEMS
outpath_DS20924=/home/data/R705A/DS20924

arr_dir=("bus" "caf" "ped" "str")
arr_noi=("BUS" "CAF" "PED" "STR")

input_root=/home/data/CHiME4_bth_5ch/
fix_dir=et05_str_simu/
fix_name=M05_441C0210_STR.wav
idx=3

i=$input_root/$fix_dir
j=$fix_name

dir=$fix_dir
input=$i'/'$j

mkdir -p $outpath_MEMS'/'$dir
mkdir -p $outpath_DS20924'/'$dir

echo ./RECORD $output_root $dir'/'$j MEMS DS20924 $input /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav
./RECORD $output_root $dir'/'$j MEMS DS20924 $input /home/data/CHiME4_Noise/longest/${arr_noi[$idx]}.wav

