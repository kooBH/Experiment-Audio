"""
Evaluate the performance of noise reduction algorithms.

Assumptions:
- noisy and estim files have same name
- Last 5 seconds of each file is noise.

"""


import os,glob
import librosa as rs
import numpy as np


dir_noisy = ""
dir_estim = ""
sr = 48000
end = -1.0 * sr
start = -4.0 * sr

list_noisy = glob.glob(os.path.join(dir_noisy,"**","*.wav"),recursive=True)


score = 0
for i,path in enumerate(list_noisy) : 
    file_name = os.path.basename(path)
    path_estim = os.path.join(dir_estim,file_name)

    noisy = rs.load(path,sr=sr, mono=True)[0]
    estim = rs.load(path_estim,sr=sr, mono=True)[0]

    # Validate noise
    noise = noisy[start:end]
    noise_estim = estim[start:end]

    power_noise =  10*np.log10(np.sum(noise**2))
    power_estim = 10*np.log10(np.sum(noise_estim**2))

    noise_reduction = power_noise - power_estim

    score += noise_reduction
score = score / len(list_noisy)

print(f"Noise Reduction {score} dB")



