import numpy as np
import math
import pdb
import librosa
import os 
import soundfile
import random
import pickle
from scipy.io import savemat
from pathlib import Path
import glob
import argparse


class Synthesize:
    def __init__(self, target_list,n1_list,n2_list, output_path):
        self.target_list = target_list
        self.n1_list = n1_list
        self.n2_list = n2_list
        self.output_path = output_path
        #self.out_n1_list = []
        #self.out_n2_list = []
        #self.out_n12_list = []
        #self.out_target_list = []
        self.tmp_n1_list = {}
        i=0
        for n1 in self.n1_list:
            i+=1
            (n1_audio,n1_fs) = librosa.load(n1,sr=16000,mono=False)
            self.tmp_n1_list[os.path.split(n1)[-1]] = n1_audio 
            print(os.path.split(n1)[-1]+str(i))

    def get(self):
        for target in self.target_list: 
            '''padding for target'''
            (t_audio,t_fs) = librosa.load(target,sr=16000)
            target_len = len(t_audio)
            noise_len = target_len + 32000
            padding = np.zeros([16000])
            t_audio = np.concatenate((padding,t_audio,padding),axis=0)

            '''select ismir04 noise'''
            while 1:
                n1 = random.choice(self.n1_list)
                n1_audio = self.tmp_n1_list[os.path.split(n1)[-1]]
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
            n2 = random.choice(self.n2_list)
            (n2_audio,n2_fs) = librosa.load(n2,sr=16000)
            while 1:    
                if len(n2_audio) >= noise_len:
                    n2_audio = n2_audio[:noise_len]
                    break
                else:
                    tmp = random.choice(self.n2_list)
                    (tmp_audio,tmp_fs) = librosa.load(tmp,sr=16000)
                    n2_audio = np.concatenate((n2_audio,tmp_audio),axis=0)
            file_name = os.path.split(target)[-1]
            soundfile.write(os.path.join(self.output_path,'Clean',file_name),t_audio,16000) 
            soundfile.write(os.path.join(self.output_path,'Noise1',file_name),n1_audio.transpose(),16000) 
            soundfile.write(os.path.join(self.output_path,'Noise12',file_name),np.stack((n1_audio[0],n2_audio),axis=1),16000) 
            soundfile.write(os.path.join(self.output_path,'Noise2',file_name),np.stack((n2_audio,n2_audio),axis=1),16000) 
            #self.out_n1_list.append(n1_audio)
            #self.out_n2_list.append(np.stack((n2_audio,n2_audio),axis=0))
            #self.out_n12_list.append(np.stack((n1_audio[0],n2_audio),axis=0))
            #self.out_target_list.append(t_audio)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--ismir',type=str, required=True)
    parser.add_argument('--TIMIT',type=str, required=True)
    parser.add_argument('--CHiME',type=str, required=True)
    parser.add_argument('--output_path',type=str, required=True)
    args = parser.parse_args()
    ismir04_path = args.ismir
    TIMIT_path = args.TIMIT
    CHiME_path = args.CHiME
    output_path = args.output_path
    os.makedirs(ismir04_path,exist_ok=True)
    os.makedirs(TIMIT_path,exist_ok=True)
    os.makedirs(CHiME_path,exist_ok=True)
    os.makedirs(output_path,exist_ok=True)
    ismir04_list = [str(f) for f in glob.glob("%s/**/*.mp3" % ismir04_path)]
    TIMIT_list = [str(f) for f in glob.glob("%s/*_WAV/**/*.WAV" % TIMIT_path)]
    CHiME_list = [str(f) for f in glob.glob("%s/*_simu/*.wav" % CHiME_path)]
    CHiME_list = sorted(CHiME_list)
    pdb.set_trace() 
    synth = Synthesize(CHiME_list,ismir04_list,TIMIT_list,output_path)
    synth.get()
    
