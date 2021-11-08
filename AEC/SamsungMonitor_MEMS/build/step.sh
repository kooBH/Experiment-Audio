#!/bin/bash  


outpath=/home/mini/data/recording/20211028_AEC_Monitor_MEMS_921/
wav_root=/home/mini/data/AEC/set3/
TARGET=ismir

input="target_list.txt"

while IFS= read -r line
do
#  while :
#   do
    dir=$(echo $line| cut -d'/' -f1)
    file=$(echo $line| cut -d'/' -f2)

    mkdir -p $outpath'/'MEMS'/'$dir
    mkdir -p $outpath'/'reference'/'$dir


	echo ./RECORD $wav_root'/'Clean'/'$file  $wav_root'/'${TARGET}'/'$file   $dir $file $outpath'/'MEMS $outpath'/'DS20921 $outpath'/'dump

	./RECORD $wav_root'/'Clean'/'$file  $wav_root'/'${TARGET}'/'$file $wav_root'/'CHiME'/'$file   $dir $file $outpath'/'MEMS $outpath'/'DS20921 $outpath'/'dump
	cp $wav_root'/'${TARGET}'/'$file $outpath'/'reference'/'$dir'/'$file

 # done

    # echo $dir/$file
done < "$input"


