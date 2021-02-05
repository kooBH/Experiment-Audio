#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

#define DEVICE 19
#define DEVICE_CLEAN 20
#define DEVICE_NOISE 17

#define NORM_MUL 32765

#define CHANNELS 16
#define SAMPLERATE 16000

#define SNR 0.0


void AudioProbe();

int main(int argc, char** argv) {

  /* Init */
  const double eps = 1e-16;
  // 10^(5/10)
  
  //const double mul_noise = NORM_MUL / 3.1623;
  const double mul_noise = NORM_MUL;

  double energy_clean=0,energy_noise=0;
  double mean_energy_clean=0,mean_energy_noise=0;
  double energy_normal=0;

  short max_n=0,max_c=0;


  bool flag_noise=false;
  // clean : input.wav output.wav
  // noisy : input.wav output.wav noise.wav
  if(argc > 3)
    flag_noise = true;

  //dual recorder("dir","name",CHANNELS,DEVICE_MLDR,DEVICE_Conex,SAMPLERATE);
  Recorder recorder("dir","name",CHANNELS,DEVICE,SAMPLERATE);
  RtOutput speaker_c(DEVICE_CLEAN,1,SAMPLERATE,48000,128,512);
  RtOutput speaker_n(DEVICE_NOISE,1,SAMPLERATE,48000,128,512);

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
  fread(buf_c,sizeof(short),nRead_c,fp_c);
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
    fp_n = fopen(argv[3],"rb");
    fseek(fp_n, 0L, SEEK_END);
    nRead_n= ftell(fp_n)-44;  // 44 : WAV format head size

    int noise_length = nRead_c/2 + int(16000*2.2);

    /* Randomly choose noise interval */
    std::random_device rd;
    std::mt19937 gen(rd());
//    gen.seed(3);

    std::uniform_int_distribution<int> rand_noise(0,nRead_n/2-noise_length);

    noise_start = rand_noise(gen)*2; // 2bytes sample

    fseek(fp_n,noise_start,SEEK_SET);


    buf_n =  new short[noise_length];
    fread(buf_n,sizeof(short),noise_length,fp_n);
    /* Normalize N */
    for(int i=0;i<noise_length;i++)
      if(max_n < std::abs(buf_n[i]))
        max_n = abs(buf_n[i]);
    for(int i=0;i<noise_length;i++)
      ;//    buf_n[i] = (short)( ((double)(buf_n[i])/(max_n+eps))*mul_noise);

    /* E noise */
    for(int i=0;i<noise_length;i++)
      energy_noise += buf_n[i]*buf_n[i];
    mean_energy_noise = energy_noise/noise_length;

    /* SNR */
    energy_normal = std::sqrt(mean_energy_clean)/std::sqrt(mean_energy_noise);
    double SNRweight = energy_normal/(std::sqrt(std::pow(10,SNR/10)));

    for(int i=0;i<noise_length;i++)
      buf_n[i]*=SNRweight;

    speaker_n.FullBufLoad(buf_n,noise_length);
    printf("noisy speech loaded\n");
  }

  std::thread *thread_record;
  printf("NOTE::INITALIZED\n");

  /* Routine */

  thread_record= new std::thread(&Recorder::Process,&recorder,argv[2]);

  printf("NOTE::RECORDING STARTED\n");
  if(flag_noise)
    speaker_n.Start();
//  SLEEP(500);
  // keyword
  speaker_c.FullBufLoad(buf_c,nRead_c/2);
  //SLEEP(500);
  speaker_c.Start();
  printf("NOTE::PLAYING SOUND\n");
  speaker_c.Wait();
  if(flag_noise)
    speaker_n.Wait();

  SLEEP(1000);
  printf("STOP RECORDING\n");
  recorder.Stop();

  delete[] buf_c;
  if(buf_n) delete[] buf_n;
  return 0;
}

