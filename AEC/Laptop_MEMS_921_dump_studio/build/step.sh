#!/bin/bash  


outpath=/home/mini/data/recording/20210918_AEC_MEMS_921/
wav_root=/home/mini/data/AEC/longer_noise/
TARGET=Noise1

input="target_list.txt"

sleep 30

while IFS= read -r line
do
    dir=$(echo $line| cut -d'/' -f1)
    file=$(echo $line| cut -d'/' -f2)

    mkdir -p $outpath'/'MEMS'/'$dir
    mkdir -p $outpath'/'DS20921'/'$dir
    mkdir -p $outpath'/'reference'/'$dir
    mkdir -p $outpath'/'dump'/'$dir


	echo ./RECORD $wav_root'/'Clean'/'$file  $wav_root'/'${TARGET}'/'$file   $dir $file $outpath'/'MEMS $outpath'/'DS20921 $outpath'/'dump

	./RECORD $wav_root'/'Clean'/'$file  $wav_root'/'${TARGET}'/'$file   $dir $file $outpath'/'MEMS $outpath'/'DS20921 $outpath'/'dump
	cp $wav_root'/'${TARGET}'/'$file $outpath'/'reference'/'$dir'/'$file

    # echo $dir/$file
done < "$input"
