import numpy as np
import glob, os
import librosa as rs
import soundfile as sf


"""
Assumptions: 

last 5 second of each segment is noise
each segment has 3 seconds of silent interval

"""


target = ""
version = ""
dir_out = ""
sr = 48000
sec_noise = 5
sec_interval = 3

x = rs.load(target,sr=48000, mono=True)[0]

segs = rs.effects.split(x,top_db=20, frame_length=2048, hop_length=512)
n_seg = len(segs)

# Validate each segment

valid_segs = []

for i in range(n_seg) :
    start = segs[i][0]
    end = segs[i][1]

    # validate seg length
    if end - start < sr * sec_noise :
        print(f"Segment {i} is too short | {end - start}")
        continue

    if i < n_seg - 1 :
        next_start = segs[i+1][0]
        interval = next_start - end
        if interval < sr * sec_interval :
            print(f"Segment {i} is too close to next segment | {interval}")
            continue
    
    valid_segs.append(segs[i])

print(f"Valid segments : {len(valid_segs)}/{len(segs)}")

# Save valid segments 
os.makedirs(dir_out,exist_ok=True)
for idx, seg in enumerate(valid_segs) :
    file_name = f"{version}_{idx}.wav"
    file_path = os.path.join(dir_out,file_name)
    sf.write(file_path, seg, sr)




