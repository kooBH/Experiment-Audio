import numpy as np
import glob, os
import librosa as rs
import soundfile as sf
from tqdm.auto import tqdm


"""
Assumptions: 

last 5 second of each segment is noise
each segment has 3 seconds of silent interval

"""

target = "D:/Data/mobile/20250514_약자_TTA_TC5/A23_20250514_kspon_4sec.m4a"
#target = "D:/Data/mobile/20250514_약자_TTA_TC5/A23_20250514_librispeech_test_clean.m4a"
version = os.path.basename(target).split(".")[0]
dir_out = "D:/Data/mobile/20250514_약자_TTA_TC5_eval"
sr = 48000
sec_noise = 5
sec_interval = 2 # 3-1 
init_pause = 60

min_seg = 100
max_seg = 400


x = rs.load(target,sr=48000, mono=False)[0]
x = x[:,init_pause*sr:]
print(x.shape)

segs = rs.effects.split(x,top_db=30, frame_length=2048, hop_length=512)
#print(segs)
n_seg = len(segs)
print(n_seg)

if n_seg > max_seg :
    print(f"Too many segments | {n_seg} > {max_seg}")
    exit(0)
if n_seg < min_seg :
    print(f"Too few segments | {n_seg} < {min_seg}")
    exit(0)

# Validate each segment

valid_segs = []

for i in range(n_seg) :
    start = segs[i][0]
    end = segs[i][1]

    # validate seg length
    if end - start < sr * sec_noise :
        print(f"Segment {i}[{start}:{end}=[{start/sr:.1f}:{end/sr:.1f}] is too short | {end - start} samples | {(end - start)/sr:.1f} sec")
        continue

    if i < n_seg - 1 :
        next_start = segs[i+1][0]
        interval = next_start - end
        if interval < sr * sec_interval :
            print(f"Segment {i}[{start}:{end}]=[{start/sr:.1f}:{end/sr:.1f}] is too close to next segment | {interval} samples | {interval/sr:.1f} sec")
            continue
    
    valid_segs.append(segs[i])

print(f"Valid segments : {len(valid_segs)}/{len(segs)}")

# Save valid segments 
os.makedirs(dir_out,exist_ok=True)
for idx, seg in tqdm(enumerate(valid_segs)) :
    file_name = f"{version}_{idx}.wav"
    file_path = os.path.join(dir_out,file_name)
    sf.write(file_path, x[:,seg[0]:seg[1]].T, sr)




