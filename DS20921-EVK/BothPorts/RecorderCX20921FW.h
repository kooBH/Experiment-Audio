#ifndef _H_RECORDER_DS20921_FIRMWARE_
#define _H_RECORDER_DS20921_FIRMWARE_

/*
OUTPUT SIGNAL
6-channel 16kHz

1ch - original left
2ch - original right
3ch - reference left
4ch - reference right
5ch - mixed original mic
6ch - processed output

OUTPUT FILES
raw : 2ch original signal
ref : 2ch reference signal
out : 1ch processed signal
*/

/* General */
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <thread>

/* Process */
#include "RtInput.h"
#include "WAV.h"
#include "time.h"

class RecorderCX20921FW{

private:
  /* General */
  std::atomic<bool>flag_recording;

  std::condition_variable cv;
  std::mutex mtx;
  std::unique_lock<std::mutex> *lock;

  /* Process */
  short* raw; 
  short* buf_original, *buf_reference,*buf_processed; 

  // fundamental parameters
  int input_size;
  int device,channels,samplerate,shift_size,frame_size;
  RtAudio audio;

  std::string dir;
  std::string name;

  RtInput *input;
  WAV* output;
  WAV* original;
  WAV* reference;
  WAV* processed;
  char file_name[64];

  bool flag_finish;

  inline void OpenDevice(int device);
  inline void Wait();
public:
  /* General */
  inline RecorderCX20921FW(std::string dir,std::string path,int channels,int device,int samplerate);
  inline ~RecorderCX20921FW();
  /* Process */
  inline void Process();
  inline void Stop();
};

RecorderCX20921FW::RecorderCX20921FW(std::string dir_, std::string name_, int channels_,int device,int samplerate_){
  dir = dir_;
  name = name_;
  channels = channels_;
  samplerate = samplerate_;
  input_size = 2048;
  shift_size = 128;
  frame_size = 512;


  /* General */
  flag_recording.store(false);
  flag_finish = false;
  lock = new std::unique_lock<std::mutex>(mtx);

  /* PROCESS */
  OpenDevice(device);
  output = new WAV(channels,samplerate);
  original = new WAV(2,samplerate);
  reference = new WAV(2,samplerate);
  processed = new WAV(1,samplerate);
  raw = new short[channels*shift_size];
  buf_original = new short[2*shift_size];
  buf_reference= new short[2*shift_size];
  buf_processed= new short[shift_size];
}

RecorderCX20921FW::~RecorderCX20921FW() {

  /* Process */
  delete[] raw;
  delete input;
  delete original;
  delete reference;
  delete processed;
}


void RecorderCX20921FW::OpenDevice(int device_) {
  device= device_;
  input= new RtInput(device,channels,samplerate,shift_size,frame_size,input_size);
}

void RecorderCX20921FW::Process(){
  flag_finish= false;
  flag_recording.store(true);

  input->Start();

  std::string original_path  = dir + "/original/"+name;
  std::string reference_path = dir + "/reference/"+name;
  std::string processed_path = dir + "/processed/"+name;
  std::string output_path = dir + "/output/"+name;

  //output->NewFile(output_path.c_str());
  original->NewFile(original_path.c_str());
  //reference->NewFile(reference_path.c_str());
  //processed->NewFile(processed_path.c_str());

  // resolve all left buffers
  while(flag_recording.load() || input->data.stock.load() > shift_size){
    // enough buffer to read
    if (input->data.stock.load() > shift_size) {
      input->GetBuffer(raw);

     for(int i =0; i<shift_size*channels;i+=channels){
       int j = i/channels;
//	   printf("%d %d\n",i,j);
       buf_original[2*j] = raw[i];
       buf_original[2*j+1] = raw[i+1];
       //buf_reference[2*j] = raw[i+2];
       //buf_reference[2*j+1] = raw[i+3];
       //buf_processed[j] = raw[i+5];
     }

      original->Append(buf_original,2*shift_size);
      // not  enough buffer
    }else {
      SLEEP(10);
    }
  }
  
  input->Stop();

  original->Finish();
  flag_finish = true;
}

void RecorderCX20921FW::Stop(){
  flag_recording.store(false);
  while(! flag_finish){
    SLEEP(100);
  }
}

#endif
