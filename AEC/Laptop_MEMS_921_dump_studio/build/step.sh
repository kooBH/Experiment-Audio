#!/bin/bash  

input="target_list.txt"
while IFS= read -r line
do
    echo "$line"
done < "$input"
