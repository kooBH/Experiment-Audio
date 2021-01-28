#include "RtOutput.h"
#include "RecorderCX20921.h"
#include "RecorderCX20921FW.h"

#include <math.h>
#include <string.h>

#define DEVICE_LEFT 19
#define DEVICE_RIGHT 19
#define DEVICE_CLEAN 20
#define DEVICE_NOISE 3

#define CHANNEL_LEFT 2
#define CHANNEL_RIGHT 6
#define SAMPLERATE 16000

#define NORM_MUL 32765
#define SNR 5.0


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

  bool flag_noise=true;

  /* Recorder(<root_dir>,<file_name>)*/
  RecorderCX20921 recorder_left(argv[3],argv[4],CHANNEL_LEFT,DEVICE_LEFT,SAMPLERATE);
  RecorderCX20921FW recorder_right(argv[3],argv[4],CHANNEL_RIGHT,DEVICE_RIGHT,SAMPLERATE);
  RtOutput speaker_c(DEVICE_CLEAN,1,SAMPLERATE,48000,128,512);
  RtOutput speaker_n(DEVICE_NOISE,2,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr,*fp_n=nullptr,*fp_a=nullptr;
  unsigned int nRead_c = 0,nRead_n = 0,nRead_a=0;
  unsigned int noise_start;
  short *buf_c=nullptr,*buf_n=nullptr,*buf_a;

  printf("Clean speech loaded\n");

  fp_c = fopen(argv[1], "rb");

  fseek(fp_c, 0L, SEEK_END);
  nRead_c= ftell(fp_c)-44;  // 44 : WAV format head size
  fseek(fp_c, 44, SEEK_SET);

  buf_c =  new short[nRead_c/2];
  fread(buf_c,sizeof(short),nRead_c/2,fp_c);
  /* Normalize C */
  for(int i=0;i<nRead_c/2;i++)
    if(max_c < std::abs(buf_c[i]))
      max_c = abs(buf_c[i]);
  for(int i=0;i<nRead_c/2;i++)
   buf_c[i] = (short)( ((double)(buf_c[i])/(max_c+eps))*NORM_MUL);

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

    speaker_n.FullBufLoad(buf_n,noise_length);
    printf("noisy speech loaded\n");
  }

  std::thread *thread_record_left;
  std::thread *thread_record_right;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  speaker_c.FullBufLoad(buf_c, nRead_c / 2);
  thread_record_left= new std::thread(&RecorderCX20921::Process,&recorder_left);
  thread_record_right= new std::thread(&RecorderCX20921FW::Process,&recorder_right);

  printf("NOTE::RECORDING STARTED\n");
  if(flag_noise)
    speaker_n.Start();
  speaker_c.Start();
  printf("NOTE::PLAYING SOUND\n");
  speaker_c.Wait();
  if(flag_noise)
    speaker_n.Wait();

  printf("STOP RECORDING\n");
  recorder_left.Stop();
  recorder_right.Stop();

  delete[] buf_c;
  if(buf_n) delete[] buf_n;
  return 0;
}
