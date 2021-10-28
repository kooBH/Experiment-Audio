#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

#define DEVICE_MEMS 15
#define DEVICE_921 20     // I2S RX 16K TX 48K
#define DEVICE_STUDIO_IN 16
#define DEVICE_STUDIO_OUT 17

#define NORM_MUL 32765

#define CHANNELS_MEMS 10
#define CHANNELS_921 2
#define CHANNELS_STUDIO 2
#define SAMPLERATE 16000
#define SAMPLERATE_STUDIO 48000


bool quiet = false;
double quiet_scale = 0.1;

double scale = 0.8;


void AudioProbe();

int main(int argc, char** argv) {

  /* Init */
  const double eps = 1e-16;
  // 10^(5/10)
  
  //const double mul_noise = NORM_MUL / 3.1623;
  const double mul_noise = NORM_MUL;



  bool  flag_noise = true;

  //dual recorder("dir","name",CHANNELS,DEVICE_MLDR,DEVICE_Conex,SAMPLERATE);
  Recorder recorder_MEMS(argv[6],argv[4],argv[5],CHANNELS_MEMS,DEVICE_MEMS,SAMPLERATE);
  Recorder recorder_921(argv[7],argv[4],argv[5],CHANNELS_921,DEVICE_921,SAMPLERATE);
  Recorder recorder_STUDIO(argv[8],argv[4],argv[5],CHANNELS_STUDIO,DEVICE_STUDIO_IN,SAMPLERATE_STUDIO);



  RtOutput speaker(DEVICE_STUDIO_OUT,1+1+2,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr;
  FILE *fp_a=nullptr;
  FILE *fp_n1=nullptr;
  FILE *fp_n2=nullptr;

  unsigned int nRead_c = 0;
  unsigned int nRead_a = 0;
  unsigned int nRead_n1 = 0;
  unsigned int nRead_n2 = 0;

  unsigned int noise_start;

  short *buf_c=nullptr;
  short *buf_n1=nullptr;
  short *buf_n2=nullptr;
  short *buf_o =nullptr;

  short max_n1=0;
  short max_n2=0;
  short max_c=0;

  printf("Clean speech loaded\n");

  fp_c = fopen(argv[1], "rb");

  fseek(fp_c, 0L, SEEK_END);
  nRead_c= ftell(fp_c)-44;  // 44 : WAV format head size
  fseek(fp_c, 44, SEEK_SET);

  buf_c =  new short[nRead_c/2];
  fread(buf_c,sizeof(short),nRead_c/2,fp_c);

  /* Noise 1  : AEC 2ch */
  fp_n1 = fopen(argv[2],"rb");
  fseek(fp_n1, 0L, SEEK_END);
  nRead_n1= ftell(fp_n1)-44;  // 44 : WAV format head size
  fseek(fp_n1, 44, SEEK_SET);
  int noise_length1 = nRead_n1 / 2 ;
  buf_n1 =  new short[noise_length1];
  fread(buf_n1,sizeof(short),noise_length1,fp_n1);

  /* Noise 2 : CHiME4 stationary */
  fp_n2 = fopen(argv[3],"rb");
  fseek(fp_n2, 0L, SEEK_END);
  nRead_n2= ftell(fp_n2)-44;  // 44 : WAV format head size
  fseek(fp_n2, 44, SEEK_SET);
  int noise_length2 = nRead_n2 / 2 ;
  buf_n2 =  new short[noise_length2];
  fread(buf_n2,sizeof(short),noise_length2,fp_n2);

  /* Output */
  buf_o = new short[nRead_c+noise_length1+noise_length2];

  /* Normalize N */

  for(int i=0;i<noise_length1;i++){
    if(max_n1 < std::abs(buf_n1[i]))
      max_n1 = abs(buf_n1[i]);
  }
  for(int i=0;i<noise_length2;i++){
    if(max_n2 < std::abs(buf_n2[i]))
      max_n2 = abs(buf_n2[i]);
  }

  // -32767 ~ 32767

  for(int i=0;i<noise_length1;i++){
    buf_n1[i] = short((buf_n1[i]/(double)max_n1)*32767);
  }
  for(int i=0;i<noise_length2;i++){
    buf_n2[i] = short((buf_n2[i]/(double)max_n2)*32767);
  }

  // Merge Noise + Clean
  for(int i=0;i<nRead_c/2;i++){
    buf_o[4*i]   = short(buf_c[i]*scale);
    buf_o[4*i+1] = short(buf_n2[i]*scale);
    buf_o[4*i+2] = short(buf_n1[2*i]*scale);
    buf_o[4*i+3] = short(buf_n1[2*i+1]*scale);
  }

  std::thread *thread_record_MEMS;
  std::thread *thread_record_921;
  std::thread *thread_record_STUDIO;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  speaker.FullBufLoad(buf_o, (nRead_c/2) + noise_length1 + noise_length2);

  thread_record_MEMS = new std::thread(&Recorder::Process,&recorder_MEMS);
  thread_record_921 = new std::thread(&Recorder::Process,&recorder_921);
  thread_record_STUDIO= new std::thread(&Recorder::Process,&recorder_STUDIO);

  // 0.5 sec sleep
  SLEEP(500);

  printf("NOTE::RECORDING STARTED\n");
  speaker.Start();
  printf("NOTE::PLAYING SOUND\n");
  speaker.Wait();

  printf("STOP RECORDING\n");
  recorder_MEMS.Stop();
  recorder_921.Stop();
  recorder_STUDIO.Stop();

  delete[] buf_c;
  delete[] buf_n1;
  delete[] buf_n2;
  delete[] buf_o;

  return 0;
}

