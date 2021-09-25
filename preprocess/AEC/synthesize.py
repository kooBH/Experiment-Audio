import numpy as np
import math
import librosa
import os 
import soundfile
import random
import glob
import argparse
from tqdm import tqdm
from multiprocessing import Pool, cpu_count

import warnings
warnings.filterwarnings("ignore")

samplerate = 16000
path_alexa  = '../../resources/Alexa_16k.wav'
len_padding_1 = int(1.0*samplerate)
len_padding_2 = int(1.0*samplerate)
len_padding_3 = int(2.0*samplerate)

padding_1 = None
padding_2 = None
padding_3 = None

ismir04_list = None
TIMIT_list = None
CHiME_list = None
noise_list = None

ismir04_audio_list  = {}
audio_alexa = None
len_alexa = None

output_path = None

padding_1 = np.zeros([len_padding_1])
padding_2 = np.zeros([len_padding_2])
padding_3 = np.zeros([len_padding_3])

parser = argparse.ArgumentParser()
parser.add_argument('--ismir',type=str, required=True)
parser.add_argument('--TIMIT',type=str, required=True)
parser.add_argument('--CHiME',type=str, required=True)
parser.add_argument('--noise',type=str, required=True)
parser.add_argument('--output_path',type=str, required=True)
args = parser.parse_args()

ismir04_path = args.ismir
TIMIT_path = args.TIMIT
CHiME_path = args.CHiME
noise_path = args.noise
output_path = args.output_path

os.makedirs(output_path+'/Clean',exist_ok=True)
os.makedirs(output_path+'/ismir',exist_ok=True)
os.makedirs(output_path+'/TIMIT',exist_ok=True)
os.makedirs(output_path+'/ismir_TIMIT',exist_ok=True)
os.makedirs(output_path+'/CHiME',exist_ok=True)

ismir04_list = [str(f) for f in glob.glob("%s/**/*.mp3" % ismir04_path)]
TIMIT_list = [str(f) for f in glob.glob("%s/*_WAV/**/*.WAV" % TIMIT_path)]
CHiME_list = [str(f) for f in glob.glob("%s/*_simu/*.wav" % CHiME_path)]
noise_list = [x for x in glob.glob(os.path.join(noise_path,'*.wav'))]

audio_alexa,_ = librosa.load(path_alexa,sr=16000)
len_alexa = len(audio_alexa)

def synthesize(idx) : 
    target = CHiME_list[idx]

    '''padding for target'''
    (t_audio,t_fs) = librosa.load(target,sr=16000)
    target_len = len(t_audio)
    t_audio = np.concatenate((padding_1,audio_alexa,padding_2,t_audio,padding_3),axis=0)

    noise_len = target_len + len_padding_1 + len_padding_2 +len_padding_3 + len_alexa

    '''select ismir04 noise'''
    while 1:
        n1 = random.choice(ismir04_list)
        (n1_audio,n1_fs) = librosa.load(n1,sr=16000,mono=False)
        if n1_audio.shape[1] > noise_len:
            len_n1_audio = n1_audio.shape[1]
            break
    try:
        n1_rising_edge = math.floor(random.uniform((1/10)*len_n1_audio,(9/10)*len_n1_audio-noise_len))
    except:
        n1_rising_edge = math.floor(random.uniform(0,len_n1_audio-noise_len))
    
    n1_falling_edge = n1_rising_edge + noise_len
    n1_audio = n1_audio[:,n1_rising_edge:n1_falling_edge]

    '''select TIMIT noise, concat until it meets noise_len'''
    n2 = random.choice(TIMIT_list)
    (n2_audio,n2_fs) = librosa.load(n2,sr=16000)
    while 1:    
        if len(n2_audio) >= noise_len:
            n2_audio = n2_audio[:noise_len]
            break
        else:
            tmp = random.choice(TIMIT_list)
            (tmp_audio,tmp_fs) = librosa.load(tmp,sr=16000)
            n2_audio = np.concatenate((n2_audio,tmp_audio),axis=0)

    ## Select CHiME noise 
    n3 = random.choice(noise_list)
    n3_audio, _ = librosa.load(n3,sr=16000)

    n3_idx = np.random.randint(len(n3_audio)-noise_len)
    n3_audio = n3_audio[n3_idx:n3_idx + noise_len]

    ## normalization
    t_max = np.max(np.abs(t_audio))
    t_audio = t_audio/t_max
    t_max = np.max(np.abs(n1_audio))
    n1_audio = n1_audio/t_max
    t_max = np.max(np.abs(n2_audio))
    n2_audio = n2_audio/t_max
    t_max = np.max(np.abs(n3_audio))
    n3_audio = n3_audio/t_max

    file_name = os.path.split(target)[-1]
    soundfile.write(os.path.join(output_path,'Clean',file_name),t_audio,16000) 
    soundfile.write(os.path.join(output_path,'ismir',file_name),n1_audio.transpose(),16000) 
    soundfile.write(os.path.join(output_path,'ismir_TIMIT',file_name),np.stack((n1_audio[0],n2_audio),axis=1),16000) 
    soundfile.write(os.path.join(output_path,'TIMIT',file_name),np.stack((n2_audio,n2_audio),axis=1),16000) 
    soundfile.write(os.path.join(output_path,'CHiME',file_name),n3_audio,16000) 

if __name__ == '__main__':
    cpu_num = cpu_count()
    #synthesize(0)
    arr = list(range(len(CHiME_list)))
    with Pool(cpu_num) as p:
        r = list(tqdm(p.imap(synthesize, arr), total=len(arr),ascii=True,desc='Synthesizer'))
