import pyaudio
import librosa as rs
from pydub import AudioSegment
import time,os,glob
from player import *


dir_input = "D:/Data/KsponSpeech_eval/eval_clean"

p = pyaudio.PyAudio()

### Audio API ###

n_hostApi = p.get_host_api_count()
arr_hostApi = {}
for i in range(n_hostApi):
    info = p.get_host_api_info_by_index(i)
    arr_hostApi[str(i)] = info["name"]
    print(f"Host API {i}: {arr_hostApi[str(i)]} for {info['deviceCount']} devices")
hostAPI_index = int(input("Choose hostAPI : "))

### Output Device ###
for i in range(p.get_device_count()):
    info = p.get_device_info_by_index(i)
    if info['maxOutputChannels'] > 0:
        if hostAPI_index == info['hostApi']:
            print(f"Device {i}: {info['name']}")
            print(f" - host API: { arr_hostApi[str(info['hostApi'])]}")
            print(f" - output channels: {info['maxOutputChannels']}")

output_device_index = int(input("Choose Output device index : "))

### Init ###
stream = open_stream(output_device_index,p,n_channels=1,sr=48000)

### Play ###
list_play = glob.glob(os.path.join(dir_input,"*.pcm"))

idx = 0
for path in list_play : 
    print(path)
    # for PCM file
    audio = AudioSegment.from_file(
        path,
        format="raw",
        frame_rate=16000,  
        channels=1,      
        sample_width=2  
    )
    audio = resample(audio, target_sr = 48000)
    play_stream(stream,audio)
    # pause
    time.sleep(3)
    if idx > 3 :
        break
    idx += 1


#audio = AudioSegment.from_file("../resources/female_1.wav", format="wav")
#audio = resample(audio, target_sr = 48000)

#play_sample(audio,output_device_index,p)

#print(audio.sample_width)
#print(audio.frame_rate)
#print(audio.channels)

close_stream(stream)


### Release ###
p.terminate()