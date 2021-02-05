#!/bin/bash

### NOISY ###

input_root=/home/data/seoul_speech_reading_data
output_path=/home/data/seoul_clean

mkdir -p $output_path

for i in $(ls $input_root); do
    echo ./MEMS $input_root'/'$i $output_path'/'$i
    ./MEMS $input_root'/'$i $output_path'/'$i
 done

