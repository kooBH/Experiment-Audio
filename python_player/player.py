import pyaudio
import librosa as rs
from pydub import AudioSegment
import numpy as np

### Audio API ###

### Load data ###

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

def play_sample(audio_segment, output_device_index, pyaudioObj):
    stream = pyaudioObj.open(format=pyaudioObj.get_format_from_width(audio_segment.sample_width),
                    channels=audio_segment.channels,
                    rate=audio_segment.frame_rate,
                    output=True,
                    output_device_index=output_device_index)
    
    stream.write(audio_segment.raw_data)

    stream.stop_stream()
    stream.close()


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
