#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

#define DEVICE_MEMS 10 // USBStreamer Multi-channels
#define DEVICE_CLEAN 1 // Sound Blaster X-Fi Surround 5.1 Pro
#define DEVICE_NOISE 4 // JBL Pebbles


#define CHANNELS_MEMS 8
#define SAMPLERATE 16000
#define SNR 5.0

#define NORM_MUL 32765


bool quiet = false;
double quiet_scale = 0.1;


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

  //dual recorder("dir","name",CHANNELS,DEVICE_MLDR,DEVICE_Conex,SAMPLERATE);
  Recorder recorder_MEMS(argv[5],argv[3],argv[4],CHANNELS_MEMS,DEVICE_MEMS,SAMPLERATE);


  RtOutput speaker_c(DEVICE_CLEAN,1,SAMPLERATE,48000,128,512);
  RtOutput speaker_n(DEVICE_NOISE,2,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr,*fp_n=nullptr,*fp_a=nullptr;
  unsigned int nRead_c = 0,nRead_n = 0,nRead_a=0;
  unsigned int noise_start;
  short *buf_c=nullptr,*buf_n=nullptr,*buf_a;


  fp_c = fopen(argv[1], "rb");
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
    speaker_n.FullBufLoad(buf_n,noise_length);
    printf("noisy speech loaded\n");
  }
  if(quiet)
	for(int i;i<nRead_c/2;i++)
		buf_c[i] *=quiet_scale;

  std::thread *thread_record_MEMS;
  std::thread *thread_record_921;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  speaker_c.FullBufLoad(buf_c, nRead_c / 2);
  thread_record_MEMS = new std::thread(&Recorder::Process,&recorder_MEMS);

  printf("NOTE::RECORDING STARTED\n");
  if(flag_noise)
    speaker_n.Start();
  speaker_c.Start();
  printf("NOTE::PLAYING SOUND\n");
  speaker_c.Wait();
  if(flag_noise)
    speaker_n.Wait();

  printf("STOP RECORDING\n");
  recorder_MEMS.Stop();

  delete[] buf_c;
  if(buf_n) delete[] buf_n;
  return 0;
}

