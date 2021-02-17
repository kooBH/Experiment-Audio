#ifndef _H_TRIO_
#define _H_TRIO_

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

  // fundamental parameters
  int input_size;
  int device,channels,samplerate,shift_size,frame_size;
  RtAudio audio;

  RtInput *input;
  WAV* output;
  char file_name[64];

  bool flag_finish;

  inline void OpenDevice(int device);
  inline void Wait();
public:
  /* General */
  inline Recorder(std::string dir,std::string path,int channels,int device,int samplerate);
  inline ~Recorder();
  /* Process */
  inline void Process(std::string);
  inline void Stop();
};

Recorder::Recorder(std::string dir, std::string name, int channels_,int device,int samplerate_){
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

void Recorder::Process(std::string output_path){
  flag_finish= false;
  flag_recording.store(true);

  input->Start();

  output->NewFile(output_path.c_str());
 // output_MLDR->NewFile(file_1.c_str());

  // resolve all left buffers
  while(flag_recording.load() || input->data.stock.load() > shift_size){
    // enough buffer to read
    if (input->data.stock.load() > shift_size) {
      input->GetBuffer(raw);

      output->Append(raw,shift_size*channels);
     
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
