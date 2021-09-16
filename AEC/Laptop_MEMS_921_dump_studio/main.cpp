#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

#define DEVICE_MEMS 16
#define DEVICE_921 17     // I2S RX 16K TX 48K

#define DEVICE_STUDIO 15
#define DEVICE_OCTA 18

#define NORM_MUL 32765

#define CHANNELS_MEMS 16
#define CHANNELS_921 2
#define CHANNELS_OCTA 2
#define SAMPLERATE 16000
#define SAMPLERATE_OCTA 48000


bool quiet = false;
double quiet_scale = 0.1;

double scale = 0.7;


void AudioProbe();

int main(int argc, char** argv) {

  /* Init */
  const double eps = 1e-16;
  // 10^(5/10)
  
  //const double mul_noise = NORM_MUL / 3.1623;
  const double mul_noise = NORM_MUL;

  short max_n1=0,max_n2=0,max_c=0;


  bool  flag_noise = true;

  //dual recorder("dir","name",CHANNELS,DEVICE_MLDR,DEVICE_Conex,SAMPLERATE);
  Recorder recorder_MEMS(argv[5],argv[3],argv[4],CHANNELS_MEMS,DEVICE_MEMS,SAMPLERATE);
  Recorder recorder_921(argv[6],argv[3],argv[4],CHANNELS_921,DEVICE_921,SAMPLERATE);
  Recorder recorder_OCTA(argv[7],argv[3],argv[4],CHANNELS_OCTA,DEVICE_OCTA,SAMPLERATE_OCTA);



  RtOutput speaker(DEVICE_STUDIO,2+1,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr,*fp_n=nullptr,*fp_a=nullptr;
  unsigned int nRead_c = 0,nRead_n = 0,nRead_a=0;
  unsigned int noise_start;
  short *buf_c=nullptr,*buf_n=nullptr,*buf_o;

  printf("Clean speech loaded\n");

  fp_c = fopen(argv[1], "rb");

  fseek(fp_c, 0L, SEEK_END);
  nRead_c= ftell(fp_c)-44;  // 44 : WAV format head size
  fseek(fp_c, 44, SEEK_SET);

  buf_c =  new short[nRead_c/2];
  fread(buf_c,sizeof(short),nRead_c/2,fp_c);

  if(flag_noise){
    fp_n = fopen(argv[2],"rb");
    fseek(fp_n, 0L, SEEK_END);
    nRead_n= ftell(fp_n)-44;  // 44 : WAV format head size
    fseek(fp_n, 44, SEEK_SET);
    int noise_length = nRead_n / 2 ;
    
    buf_n =  new short[noise_length];
    fread(buf_n,sizeof(short),noise_length,fp_n);

	buf_o = new short[nRead_c+noise_length];
    /* Normalize N */
	
    for(int i=0;i<noise_length;i++){
      /*
      if(max_n1 < std::abs(buf_n[2*i]))
        max_n1 = abs(buf_n[2*i]);
      if(max_n2 < std::abs(buf_n[2*i+1]))
        max_n2 = abs(buf_n[2*i+1]);
       */

      if(max_n1 < std::abs(buf_n[i]))
        max_n1 = abs(buf_n[i]);
    }
	
    // -32767 ~ 32767

    /*
    for(int i=0;i<noise_length/2;i++){
      buf_n[2*i] = short((buf_n[2*i]/(double)max_n1)*32767);
      buf_n[2*i+1] = short((buf_n[2*i+1]/(double)max_n2)*32767);
    */
    for(int i=0;i<noise_length;i++){
      buf_n[i] = short((buf_n[i]/(double)max_n1)*32767);
	}

	// Merge Noise + Clean
	for(int i=0;i<nRead_c/2;i++){
		buf_o[3*i]   = short(buf_n[2*i]*scale);
		buf_o[3*i+1] = short(buf_n[2*i+1]*scale);
		buf_o[3*i+2] = short(buf_c[i]*scale);
	}

  }
  if(quiet)
	for(int i;i<nRead_c/2;i++)
		buf_c[i] *=quiet_scale;


  std::thread *thread_record_MEMS;
  std::thread *thread_record_921;
  std::thread *thread_record_OCTA;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  speaker.FullBufLoad(buf_o, (nRead_c/2) *3);


  thread_record_MEMS = new std::thread(&Recorder::Process,&recorder_MEMS);
  thread_record_921 = new std::thread(&Recorder::Process,&recorder_921);
  thread_record_OCTA= new std::thread(&Recorder::Process,&recorder_OCTA);

  // 0.5 sec sleep
  SLEEP(500);

  printf("NOTE::RECORDING STARTED\n");
  speaker.Start();
  printf("NOTE::PLAYING SOUND\n");
  speaker.Wait();

  printf("STOP RECORDING\n");
  recorder_MEMS.Stop();
  recorder_921.Stop();
  recorder_OCTA.Stop();

  delete[] buf_c;
  if(buf_n){ 
	  delete[] buf_n;
	  delete[] buf_o;
  }
  return 0;
}

