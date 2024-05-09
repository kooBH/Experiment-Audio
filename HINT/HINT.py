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
clean_pool = np.zeros_like(noise)
noise_pool = noise.copy()

for idx,snr in enumerate(list_SNR) : 
    # sample clean
    clean = rs.load(random.sample(list_clean,1)[0],sr=sr)[0]
    len_clean = len(clean)
    
    while idx_clean + int(sec_pad*sr) + 2*len_clean > len_audio :
        noise_pool = np.concatenate((noise_pool,noise))
        clean_pool = np.concatenate((clean_pool,np.zeros_like(noise)))
        len_audio += len_noise
    idx_clean += int(sec_pad*sr)
    # SNR
    speech_rms = np.sqrt(np.mean(clean ** 2))
    noise_rms = np.sqrt(np.mean(noise **2))
    #print("{} {}".format(speech_rms,noise_rms))
    snr_scaler = (10 ** (snr/20))* (noise_rms  / (speech_rms + 1e-13))
    clean *= snr_scaler

    clean_pool[idx_clean:idx_clean+len_clean] += clean
    
    idx_clean += 2*len_clean
    print("{} |SNR {} | idx_clean {} | len_clean {} |len_audio {} | {}".format(idx,snr,idx_clean,len_clean,len_audio,snr_scaler))

audio_pool = clean_pool + noise_pool
factor_norm = np.max(np.abs(audio_pool))
clean_pool = clean_pool/factor_norm
noise_pool = noise_pool/factor_norm
audio_pool = audio_pool/factor_norm

sf.write("HINT.wav",audio_pool,sr)
sf.write("HINT_clean.wav",clean_pool,sr)
sf.write("HINT_noise.wav",noise_pool,sr)