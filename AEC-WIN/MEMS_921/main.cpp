#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

// D:\Experiment-Audio\Clean\F01_22GC010A_BUS.wav  D:\Experiment-Audio\Noise1\F01_22GC010A_BUS.wav C:\data 20210803_MEMS 20210803_921

#define DEVICE_MEMS 13 // USBStreamer Multi-channels
#define DEVICE_921 10 //I2S RX 16K TX 48K
#define DEVICE_CLEAN 0 //  Studio-Capture  5-6 for Clean
#define DEVICE_NOISE_1 7 // Studio-Capture 1-2 to laptob
//#define DEVICE_NOISE_2 4 // Stduio-Capture 3-4 to CX921



#define CHANNELS_MEMS 16
#define CHANNELS_921 2

#define SAMPLERATE 16000
#define SNR 5.0

#define NORM_MUL 32765


bool quiet = false;
double quiet_scale = 0.1;
bool silence = false;


void AudioProbe();

int main(int argc, char** argv) {

  /* Init */
  const double eps = 1e-16;
  // 10^(5/10)
  
  //const double mul_noise = NORM_MUL / 3.1623;
  const double mul_noise = NORM_MUL;

  double energy_clean=0,energy_noise_1=0,energy_noise_2=0;
  double mean_energy_clean=0,mean_energy_noise_1=0,mean_energy_noise_2=0;
  double energy_normal_1=0,energy_normal_2=0;

  short max_n=0,max_c=0;


  bool  flag_noise = true;

   // inline Recorder(std::string dir, std::string category, std::string name, int channels, int device, int samplerate);  
  Recorder recorder_MEMS(argv[3],argv[5],argv[4],CHANNELS_MEMS,DEVICE_MEMS,SAMPLERATE);
  Recorder recorder_921(argv[3], argv[6], argv[4], CHANNELS_921, DEVICE_921, SAMPLERATE);



  RtOutput speaker_c(DEVICE_CLEAN,1,SAMPLERATE,48000,128,512);
  RtOutput speaker_n1(DEVICE_NOISE_1,2,SAMPLERATE,48000,128,512);
  //RtOutput speaker_n2(DEVICE_NOISE_2, 2, SAMPLERATE, 48000, 128, 512);


  FILE *fp_c=nullptr,*fp_n=nullptr,*fp_a=nullptr;
  unsigned int nRead_c = 0,nRead_n = 0,nRead_a=0;
  unsigned int noise_start;
  short *buf_c=nullptr,*buf_n=nullptr,*buf_a;


  
  fp_c = fopen(argv[1], "rb");

  if(!silence)
    printf("Clean speech loaded\n");

  fseek(fp_c, 0L, SEEK_END);
  nRead_c= ftell(fp_c)-44;  // 44 : WAV format head size
  fseek(fp_c, 44, SEEK_SET);

  buf_c =  new short[nRead_c/2];
  fread(buf_c,sizeof(short),nRead_c/2,fp_c);

  /* Cal Energy */
  for(int i=0;i<nRead_c/2;i++)
    energy_clean += buf_c[i]*buf_c[i];
  mean_energy_clean = energy_clean/(nRead_c/2);

  if(flag_noise){
    //printf("argv[2] %s\n", argv[2]);
    fp_n = fopen(argv[2],"rb");
    fseek(fp_n, 0L, SEEK_END);
    nRead_n= ftell(fp_n)-44;  // 44 : WAV format head size
    fseek(fp_n, 44, SEEK_SET);
    int noise_length = nRead_n / 2 ;
    
    buf_n =  new short[noise_length];
    fread(buf_n,sizeof(short),noise_length,fp_n);
    /* Normalize N */
	/*
    for(int i=0;i<noise_length;i++)
      if(max_n < std::abs(buf_n[i]))
        max_n = abs(buf_n[i]);
	*/

    /* E noise */
    for(int i=0;i<noise_length/2;i++){
      energy_noise_1 += buf_n[2*i]*buf_n[2*i];
      energy_noise_2 += buf_n[2*i+1]*buf_n[2*i+1];
	}
    mean_energy_noise_1 = energy_noise_1/(noise_length/2);
    mean_energy_noise_2 = energy_noise_2/(noise_length/2);

    /* SNR */
    energy_normal_1 = std::sqrt(mean_energy_clean)/std::sqrt(mean_energy_noise_1);
    energy_normal_2 = std::sqrt(mean_energy_clean)/std::sqrt(mean_energy_noise_2);
    double SNRweight_1 = energy_normal_1/(std::sqrt(std::pow(10,SNR/10)));
    double SNRweight_2 = energy_normal_2/(std::sqrt(std::pow(10,SNR/10)));

    for(int i=0;i<noise_length/2;i++){
      buf_n[2*i]*=SNRweight_1;
      buf_n[2*i+1]*=SNRweight_2;
	}
	if(quiet)
		for(int i;i<noise_length;i++)
			buf_n[i] *=quiet_scale;
    speaker_n1.FullBufLoad(buf_n,noise_length);
   // speaker_n2.FullBufLoad(buf_n, noise_length);

    if(!silence)
      printf("noisy speech loaded\n");
  }
  if(quiet)
	for(int i;i<nRead_c/2;i++)
		buf_c[i] *=quiet_scale;

  std::thread *thread_record_MEMS;
  std::thread *thread_record_921;
  if (!silence)
    printf("NOTE::INITALIZED\n");

  /* Routine */
  speaker_c.FullBufLoad(buf_c, nRead_c / 2);
  thread_record_MEMS = new std::thread(&Recorder::Process,&recorder_MEMS);
  thread_record_921 = new std::thread(&Recorder::Process, &recorder_921);


  if (!silence)
    printf("NOTE::RECORDING STARTED\n");
  if (flag_noise) {
    speaker_n1.Start();
   // speaker_n2.Start();
  }
  speaker_c.Start();
  if (!silence)
    printf("NOTE::PLAYING SOUND\n");
  speaker_c.Wait();
  if (flag_noise) {
    speaker_n1.Wait();
   // speaker_n2.Wait();
  }
  if (!silence)
    printf("STOP RECORDING\n");
  recorder_MEMS.Stop();
  recorder_921.Stop();
  
  if (!silence)
    printf("RELASE\n");
  delete[] buf_c;
  if(buf_n) delete[] buf_n;
  return 0;
}

