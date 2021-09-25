#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

/*
speaker 1 : 1ch clean
speaker 2 : 2ch echo
speaker 3 : 1ch (point or diffuse) noise 
--> all through STUDIO-CAPTURE

recorder 1 : MEMS  16ch
recorder 2 : 921 - Stereo AEC -reference 스피커 2 의 echo 
recorder 3 : 924 - mono AEC - 
recorder 4 : echo dump - OCTA CAPTURE
*/

/* output device */
#define DEVICE_STUDIO 17

/* input device */
#define DEVICE_OCTA 18
#define DEVICE_MEMS 19
#define DEVICE_921 21     // I2S RX 16K TX 48K
#define DEVICE_924 20    // CX20924

#define NORM_MUL 32765

#define CHANNELS_MEMS 16
#define CHANNELS_921 2
#define CHANNELS_924 2
#define CHANNELS_OCTA 2

#define SAMPLERATE 16000
#define SAMPLERATE_OCTA 48000


bool quiet = false;
double quiet_scale = 0.1;

double scale = 0.9;


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
  Recorder recorder_MEMS(argv[6],argv[4],argv[5],CHANNELS_MEMS,DEVICE_MEMS,SAMPLERATE);
  Recorder recorder_921(argv[7],argv[4],argv[5],CHANNELS_921,DEVICE_921,SAMPLERATE);
  Recorder recorder_924(argv[8],argv[4],argv[5],CHANNELS_924,DEVICE_924,SAMPLERATE);
  Recorder recorder_OCTA(argv[9],argv[4],argv[5],CHANNELS_OCTA,DEVICE_OCTA,SAMPLERATE_OCTA);



  RtOutput speaker(DEVICE_STUDIO,2+1+1,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr,*fp_n=nullptr,*fp_a=nullptr,*fp_n2=nullptr;
  unsigned int nRead_c = 0,nRead_n = 0,nRead_a=0,nRead_n2=0;
  unsigned int noise_start;
  short *buf_c=nullptr,*buf_n=nullptr,*buf_o,*buf_n2=nullptr;

  printf("Clean speech loaded\n");

  fp_c = fopen(argv[1], "rb");

  fseek(fp_c, 0L, SEEK_END);
  nRead_c= ftell(fp_c)-44;  // 44 : WAV format head size
  fseek(fp_c, 44, SEEK_SET);

  buf_c =  new short[nRead_c/2];
  fread(buf_c,sizeof(short),nRead_c/2,fp_c);

  if(flag_noise){
    /* Echo */
    fp_n = fopen(argv[2],"rb");
    fseek(fp_n, 0L, SEEK_END);
    nRead_n= ftell(fp_n)-44;  // 44 : WAV format head size
    fseek(fp_n, 44, SEEK_SET);
    int noise_length = nRead_n / 2 ;

    buf_n =  new short[noise_length];
    fread(buf_n,sizeof(short),noise_length,fp_n);

    /* Stationary noise */
    fp_n2 = fopen(argv[3],"rb");
    fseek(fp_n2, 0L, SEEK_END);
    nRead_n2= ftell(fp_n2)-44;  // 44 : WAV format head size
    fseek(fp_n2, 44, SEEK_SET);
    int noise_length2 = nRead_n2 / 2 ;

    buf_n2 =  new short[noise_length2];
    fread(buf_n2,sizeof(short),noise_length2,fp_n2);

    

	  buf_o = new short[nRead_c + noise_length + noise_length2];
    /* Normalize N */
	
    for(int i=0;i<noise_length;i++){
      if(max_n1 < std::abs(buf_n[i]))
        max_n1 = abs(buf_n[i]);
    }
    for(int i=0;i<noise_length;i++){
      buf_n[i] = short((buf_n[i]/(double)max_n1)*32767);
	  }


    for(int i=0;i<noise_length2;i++){
      if(max_n2 < std::abs(buf_n2[i]))
        max_n2 = abs(buf_n2[i]);
    }
    for(int i=0;i<noise_length2;i++){
      buf_n2[i] = short((buf_n2[i]/(double)max_n2)*32767);
	  }

	// Merge Noise + Clean
	for(int i=0;i<nRead_c/2;i++){
		buf_o[4*i]   = short(buf_n[2*i]*scale);
		buf_o[4*i+1] = short(buf_n[2*i+1]*scale);
		buf_o[4*i+2] = short(buf_c[i]*scale);
    buf_o[4*i+3] = short(buf_n2[i]*scale);
	}

  }
  if(quiet)
	for(int i;i<nRead_c/2;i++)
		buf_c[i] *=quiet_scale;


  std::thread *thread_record_MEMS;
  std::thread *thread_record_921;
  std::thread *thread_record_924;
  std::thread *thread_record_OCTA;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  speaker.FullBufLoad(buf_o, (nRead_c/2) *3);


  thread_record_MEMS = new std::thread(&Recorder::Process,&recorder_MEMS);
  thread_record_921  = new std::thread(&Recorder::Process,&recorder_921);
  thread_record_924  = new std::thread(&Recorder::Process,&recorder_924);
  thread_record_OCTA = new std::thread(&Recorder::Process,&recorder_OCTA);

  // 0.5 sec sleep
  SLEEP(500);

  printf("NOTE::RECORDING STARTED\n");
  speaker.Start();
  printf("NOTE::PLAYING SOUND\n");
  speaker.Wait();

  printf("STOP RECORDING\n");
  recorder_MEMS.Stop();
  recorder_921.Stop();
  recorder_924.Stop();
  recorder_OCTA.Stop();

  delete[] buf_c;
  if(buf_n){ 
	  delete[] buf_n;
	  delete[] buf_o;
  }
  return 0;
}

