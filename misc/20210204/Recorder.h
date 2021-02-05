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

class Recorder{

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
  std::string category;
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
  inline Recorder(std::string dir,std::string category,std::string name,int channels,int device,int samplerate);
  inline ~Recorder();
  /* Process */
  inline void Process();
  inline void Stop();
};

Recorder::Recorder(std::string dir_, std::string category_,std::string name_, int channels_,int device,int samplerate_){
  dir = dir_;
  category = category_;
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
  raw = new short[channels*shift_size];
}

Recorder::~Recorder() {

  /* Process */
  delete[] raw;
  delete input;
  delete output;
}


void Recorder::OpenDevice(int device_) {
  device= device_;
  input= new RtInput(device,channels,samplerate,shift_size,frame_size,input_size);
}

void Recorder::Process(){
  flag_finish= false;
  flag_recording.store(true);

  input->Start();

  std::string output_path = dir + "/"+category+"/"+name;

  output->NewFile(output_path.c_str());

  // resolve all left buffers
  while(flag_recording.load() || input->data.stock.load() > shift_size){
    // enough buffer to read
    if (input->data.stock.load() > shift_size) {
      input->GetBuffer(raw);

      output->Append(raw,channels*shift_size);
      // not  enough buffer
    }else {
      SLEEP(10);
    }
  }
  
  input->Stop();

  output->Finish();
  flag_finish = true;
}

void Recorder::Stop(){
  flag_recording.store(false);
  while(! flag_finish){
    SLEEP(100);
  }
}

#endif
