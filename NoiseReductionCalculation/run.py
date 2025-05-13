import pyaudio
from pydub import AudioSegment
import time,os,glob
import librosa as rs
import numpy as np

def resample(audio,target_sr = 48000) : 
    try : 
        samples = np.array(audio.get_array_of_samples()).astype(np.float32)
    except ValueError as e :
        samples = np.frombuffer(audio.raw_data[:-1], dtype=np.int16)    
        samples = samples.astype(np.float32)
    channels = audio.channels
    orig_sr = audio.frame_rate
    samples /= np.iinfo(np.int16).max  # int16의 최대값으로 정규화

    if channels == 2:
        samples = samples.reshape((-1, 2)).T  # (2, N) 형태로 변환

    if channels == 2:
        left_channel = rs.resample(samples[0], orig_sr=orig_sr, target_sr=target_sr)
        right_channel = rs.resample(samples[1], orig_sr=orig_sr, target_sr=target_sr)
        resampled_samples = np.vstack([left_channel, right_channel]).T  
    else:
        resampled_samples = rs.resample(samples, orig_sr=orig_sr, target_sr=target_sr)

    resampled_samples = (resampled_samples * np.iinfo(np.int16).max).astype(np.int16)
    if channels == 2:
        resampled_audio = AudioSegment(
            resampled_samples.tobytes(),
            frame_rate=target_sr,
            sample_width=resampled_samples.dtype.itemsize,
            channels=2
        )
    else:
        resampled_audio = AudioSegment(
            resampled_samples.tobytes(),
            frame_rate=target_sr,
            sample_width=resampled_samples.dtype.itemsize,
            channels=1
        )
    return resampled_audio

def open_stream(output_device_index,pyaudioObj,n_channels=2,sr=48000):
    stream = pyaudioObj.open(
        format=pyaudioObj.get_format_from_width(2),
        channels=n_channels,
        rate=sr,
        output=True,
        output_device_index=output_device_index)
    return stream
    
def play_stream(stream, audioSegment):
    stream.write(audioSegment.raw_data)
    
def close_stream(stream):
    stream.stop_stream()
    stream.close()

if __name__ == "__main__" : 
    play_librispeech = True

    if play_librispeech :
        dir_input = "G:/test-clean" # .flac
        format = "flac"
    else :
        dir_input = "G:/KsponSpeech_eval_clean_wav_4sec" # .wav
        format = "wav"

    list_play = glob.glob(os.path.join(dir_input,"**",f"*.{format}"),recursive=True)

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
    sr = 48000
    stream = open_stream(output_device_index,p,n_channels=2,sr=sr)
    len_noise = sr*5
    gain = 0.5

    ### Play ###
    idx = 0
    for path in list_play : 

        # stereo output
        speech = AudioSegment.from_file(
            path,
            format=format,
            channels=2,      
            sample_width=2  
        )
        speech = resample(speech, target_sr = sr)
        duration = int(speech.duration_seconds *sr)
        print(f"{path} | {duration}")

        audio = np.zeros((int(duration)+len_noise,2), dtype=np.int16)

        audio[:duration,0] = speech.get_array_of_samples()[:duration]
        audio[:,1] = (np.random.randint(-32768, 32767, size=(int(duration)+len_noise,)) * gain).astype(np.int16)

        audio = AudioSegment(
            data=audio.tobytes(),
            sample_width=2,
            frame_rate=sr,
            channels=2
            )

        play_stream(stream,audio)
        # pause
        time.sleep(3)
        if idx > 3 :
            break
        idx += 1

    close_stream(stream)


    ### Release ###
    p.terminate()