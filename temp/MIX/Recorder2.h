#ifndef _H_TRIO2_
#define _H_TRIO2_

/* General */
#include <condition_variable>
#include <mutex>
#include <atomic>

/* Process */
#include "RtInput.h"
#include "WAV.h"
#include "time.h"

class Recorder2{

private:
  /* General */
  std::atomic<bool>flag_recording;

  std::condition_variable cv;
  std::mutex mtx;
  std::unique_lock<std::mutex> *lock;

  std::string dir;
  std::string name;

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
  inline Recorder2(std::string dir,std::string path,int channels,int device,int samplerate);
  inline ~Recorder2();
  /* Process */
  inline void Process();
  inline void Stop();
};

Recorder2::Recorder2(std::string dir_, std::string name_, int channels_,int device,int samplerate_){
  channels = channels_;
  samplerate = samplerate_;
  input_size = 2048;
  shift_size = 128;
  frame_size = 512;

  dir = dir_;
  name = name_;


  /* General */
  flag_recording.store(false);
  flag_finish = false;
  lock = new std::unique_lock<std::mutex>(mtx);

  /* PROCESS */
  OpenDevice(device);
  output = new WAV(channels,samplerate);
  raw = new short[channels*shift_size];
}

Recorder2::~Recorder2() {

  /* Process */
  delete[] raw;
  delete input;
  delete output;
}


void Recorder2::OpenDevice(int device_) {
  device= device_;
  input= new RtInput(device,channels,samplerate,shift_size,frame_size,input_size);
}

void Recorder2::Process(){
  flag_finish= false;
  flag_recording.store(true);

  input->Start();
  
  std::string path = dir + "/AKG/"+name;

  output->NewFile(path.c_str());
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

void Recorder2::Stop(){
  flag_recording.store(false);
  while(! flag_finish){
    SLEEP(100);
  }
}

#endif
