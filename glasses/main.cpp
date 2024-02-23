#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

#define DEVICE_1 9
#define DEVICE_CLEAN 0



#define NORM_MUL 32767
#define CHANNELS_1 8
#define SAMPLERATE 16000



void AudioProbe();

/* 
 *	argv[1] : output_root  : /home/data/DS20924
 *	argv[2] : file name     :  F10234023.wav
 *	argv[3] : clean_path
 * */

int main(int argc, char** argv) {
  printf("********************\n");

  /* Init */
  const double eps = 1e-16;
  // 10^(5/10)
  
  //const double mul_noise = NORM_MUL / 3.1623;
  const double mul_noise = NORM_MUL;

  double energy_clean=0,energy_noise_1=0,energy_noise_2=0;
  double mean_energy_clean=0,mean_energy_noise_1=0,mean_energy_noise_2=0;
  double energy_normal_1=0,energy_normal_2=0;

  short max_n=0,max_c=0,max_a=0;


  bool flag_noise=false;

  /* Recorder(<root_dir>,<file_name>)*/
  Recorder recorder_MEMS(argv[1],argv[2],CHANNELS_1,DEVICE_1,SAMPLERATE);
  RtOutput speaker_c(DEVICE_CLEAN,1,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr;
  unsigned int nRead_c = 0;
  short *buf_c=nullptr;

  fp_c = fopen(argv[3], "rb");
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

  printf("Clean speech loaded\n");


  std::thread *thread_record_1;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  thread_record_1= new std::thread(&Recorder::Process,&recorder_MEMS);

  printf("NOTE::RECORDING STARTED\n");

  SLEEP(300);

  speaker_c.FullBufLoad(buf_c, nRead_c / 2);
  printf("NOTE::PLAYING SOUND\n");
  speaker_c.Start();

  speaker_c.Wait();

  printf("STOP RECORDING\n");
  recorder_MEMS.Stop();
  delete[] buf_c;
  return 0;
}

