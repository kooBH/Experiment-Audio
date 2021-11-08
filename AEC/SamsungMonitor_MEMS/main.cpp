#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

#define DEVICE_MEMS 18
#define DEVICE_MONITOR 3
#define DEVICE_STUDIO_OUT 17

#define NORM_MUL 32765

#define CHANNELS_MEMS 10
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



  RtOutput speaker_STUDIO(DEVICE_STUDIO_OUT,1,SAMPLERATE,48000,128,512);
  RtOutput speaker_monitor(DEVICE_MONITOR,2,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr;
  FILE *fp_a=nullptr;
  FILE *fp_n1=nullptr;
  FILE *fp_n2=nullptr;

  unsigned int nRead_c = 0;
  unsigned int nRead_a = 0;
  unsigned int nRead_n1 = 0;

  unsigned int noise_start;

  short *buf_c=nullptr;
  short *buf_n1=nullptr;
  short *buf_o1 =nullptr;
  short *buf_o2 =nullptr;

  short max_n1=0;
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

  /* Output */
  buf_o1 = new short[nRead_c];
  buf_o2 = new short[noise_length1];

  /* Normalize N */

  for(int i=0;i<noise_length1;i++){
    if(max_n1 < std::abs(buf_n1[i]))
      max_n1 = abs(buf_n1[i]);
  }

  // -32767 ~ 32767
  printf("max n1 : %d\n",max_n1);

  for(int i=0;i<noise_length1;i++){
    buf_n1[i] = short((buf_n1[i]/(double)max_n1)*32767);
  }

    // Merge Noise + Clean
  /*
  for(int i=0;i<nRead_c/2;i++){
    buf_o[4*i+2]   = short(buf_c[i]*scale);
    buf_o[4*i+3] = short(buf_n2[i]*scale);
    buf_o[4*i] = short(buf_n1[2*i]*scale);
    buf_o[4*i+1] = short(buf_n1[2*i+1]*scale);
  }
  */

  // Merge Noise + Clean
  for(int i=0;i<nRead_c/2;i++){
    buf_o1[i] = short(buf_c[i]*scale);
   }
  for(int i=0;i<nRead_c/2;i++){
    buf_o2[2*i] = short(buf_n1[2*i]*scale);
    buf_o2[2*i+1] = short(buf_n1[2*i+1]*scale);
   }
 

  std::thread *thread_record_MEMS;
  printf("NOTE::INITALIZED\n");

  // Dump Output
  /* 
  WAV temp(4,16000);
  temp.NewFile("temp.wav");
  temp.Append(buf_o,4*(nRead_c/2));
  temp.Finish();
  */

  /* Routine */
  speaker_STUDIO.FullBufLoad(buf_o1, (nRead_c/2));
  speaker_monitor.FullBufLoad(buf_o2, noise_length1);

  thread_record_MEMS = new std::thread(&Recorder::Process,&recorder_MEMS);

  // 0.5 sec sleep
  SLEEP(500);

  printf("NOTE::RECORDING STARTED\n");
  speaker_STUDIO.Start();
  speaker_monitor.Start();
  printf("NOTE::PLAYING SOUND\n");
  speaker_STUDIO.Wait();
  speaker_monitor.Wait();

  printf("STOP RECORDING\n");
  recorder_MEMS.Stop();

  delete[] buf_c;
  delete[] buf_n1;

  return 0;
}

