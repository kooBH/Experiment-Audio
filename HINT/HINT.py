import librosa as rs
import soundfile as sf
import numpy as np
import glob,os
import random

sec_pad = 1.0
sr = 16000
list_SNR = [30,25,20,15,10,5,3,0,-3,-5,-7,-10]


### Prep Data
# init 
noise = rs.load("noise/whitenoise_uniform.wav",sr=sr)[0]


list_clean = glob.glob(os.path.join("clean","*.wav"))

len_noise = len(noise) 
len_audio = len_noise
idx_clean = 0
audio_pool = noise.copy()

for idx,snr in enumerate(list_SNR) : 
    # sample clean
    clean = rs.load(random.sample(list_clean,1)[0],sr=sr)[0]
    len_clean = len(clean)
    
    while idx_clean + int(sec_pad*sr) + 2*len_clean > len_audio :
        audio_pool = np.concatenate((audio_pool,noise))
        len_audio += len_noise
    idx_clean += int(sec_pad*sr)
    # SNR
    speech_rms = np.sqrt(np.mean(clean ** 2))
    noise_rms = np.sqrt(np.mean(noise **2))
    #print("{} {}".format(speech_rms,noise_rms))
    snr_scaler = (10 ** (snr/20))* (noise_rms  / (speech_rms + 1e-13))
    clean *= snr_scaler

    audio_pool[idx_clean:idx_clean+len_clean] += clean
    
    idx_clean += 2*len_clean
    print("{} |SNR {} | idx_clean {} | len_clean {} |len_audio {} | {}".format(idx,snr,idx_clean,len_clean,len_audio,snr_scaler))
    
audio_pool = audio_pool/np.max(np.abs(audio_pool))

sf.write("HINT.wav",audio_pool,sr)