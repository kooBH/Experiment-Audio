#ifndef _H_RECORDER_DS20924_FIRMWARE_
#define _H_RECORDER_DS20924_FIRMWARE_

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

class DS20924FW{

private:
  /* General */
  std::atomic<bool>flag_recording;

  std::condition_variable cv;
  std::mutex mtx;
  std::unique_lock<std::mutex> *lock;

  /* Process */
  short* raw; 
  short* raw_out; 
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
  char file_name[64];

  bool flag_finish;

  inline void OpenDevice(int device);
  inline void Wait();
public:
  /* General */
  inline DS20924FW(std::string dir,std::string category,std::string name,int channels,int device,int samplerate);
  inline ~DS20924FW();
  /* Process */
  inline void Process();
  inline void Stop();
};

DS20924FW::DS20924FW(std::string dir_, std::string category_,std::string name_, int channels_,int device,int samplerate_){
  dir = dir_;
  category = category_;
  name = name_;
  channels = channels_;
  samplerate = samplerate_;
  input_size = 2048;
  shift_size = 300;
  frame_size = 512;


  /* General */
  flag_recording.store(false);
  flag_finish = false;
  lock = new std::unique_lock<std::mutex>(mtx);

  /* PROCESS */
  OpenDevice(device);
  output = new WAV(6,16000);
  raw = new short[channels*shift_size];
  raw_out = new short[2*shift_size];
}

DS20924FW::~DS20924FW() {

  /* Process */
  delete[] raw;
  delete[] raw_out;
  delete input;
  delete output;
}


void DS20924FW::OpenDevice(int device_) {
  device= device_;
  input= new RtInput(device,channels,samplerate,shift_size,frame_size,input_size);
}

void DS20924FW::Process(){
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

	  int iter_idx;
	  int iter_raw=0;

	  /* 0 1 2 3 4 5 / 6 7 8 9 10 11
	   * a b a b a b / a b a b a  b
	   * 1 2 3 4 5 6 / 1 2 3 4 5  6 
	   *
	   * 
	   *
	   *
	   * */

	  for(int i=0;i<2*shift_size;i+=6){
		  iter_idx=i/6;
		 // printf("%d %d %d\n",i,iter_idx,iter_raw);
		  raw_out[6*iter_idx] = raw[iter_raw];
		  raw_out[6*iter_idx+1] = raw[iter_raw+1];
		  raw_out[6*iter_idx+2] = raw[iter_raw+2];
		  raw_out[6*iter_idx+3] = raw[iter_raw+3];
		  raw_out[6*iter_idx+4] = raw[iter_raw+4];
		  raw_out[6*iter_idx+5] = raw[iter_raw+5];
		  iter_raw+=6;
	  }
	  
      output->Append(raw_out,6*100);
      // not  enough buffer
    }else {
      SLEEP(10);
    }
  }
  
  input->Stop();

  output->Finish();
  flag_finish = true;
}

void DS20924FW::Stop(){
  flag_recording.store(false);
  while(! flag_finish){
    SLEEP(100);
  }
}

#endif
