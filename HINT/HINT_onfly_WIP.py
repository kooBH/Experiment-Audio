import pyaudio
import librosa as rs
import numpy as np

p = pyaudio.PyAudio()
info = p.get_host_api_info_by_index(0)
numdevices = info.get('deviceCount')

sec_pad = 1.0
sr = 16000
list_SNR = [30,25,20,15,10,5,3,0,-3,-5,-7,-10]


### Get input devices ###
cmd_bad = True
while(cmd_bad) : 
    for i in range (0,numdevices):
        if p.get_device_info_by_host_api_device_index(0,i).get('maxInputChannels')>0:
                print("Input Device ID ", i, " - ", p.get_device_info_by_host_api_device_index(0,i).get('name'))

        if p.get_device_info_by_host_api_device_index(0,i).get('maxOutputChannels')>0:
                print("Output Device ID ", i, " - ", p.get_device_info_by_host_api_device_index(0,i).get('name'))

    cmd = input("Type Device ID\n")
    if cmd.isdigit() :
        cmd = int(cmd)
        
        if cmd >= 0 and cmd <= numdevices :
            cmd_bad = False
        
    print("Type valid id : ",cmd)

idx_device = cmd
devinfo = p.get_device_info_by_index(idx_device)
print("Selected device is ",devinfo.get('name'))
print(devinfo)

max_out_channels = devinfo["maxOutputChannels"]
sr = devinfo["defaultSampleRate"]

stream = p.open(format=p.get_format_from_width(pyaudio.paFloat32),
                    channels=1,
                    rate=int(sr),
                    output_device_index=idx_device,
                    output=True)                         

### Prep Data
noise = rs.load("noise/whitenoise_uniform.wav",sr=sr)[0]
#print(noise.shape)
len_noise = len(noise) 


len_audio = len_noise
audio_pool = noise

for snr in list_SNR : 
    # sample clean
    
    # get interval for clean

    speech_rms = np.sqrt(np.mean(x ** 2))
    noise_rms = np.sqrt(np.mean(n **2))
    snr_scaler = speech_rms / (10 ** (SNR/20)) / (noise_rms + 1e-13)
    x /= snr_scaler

    
            

### Play Data             
stream.write(src_noise.astype(np.float32).tostring())


### Release
stream.stop_stream()
stream.close()                                     
                         
p.terminate()                         