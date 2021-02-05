#include "RtOutput.h"
#include "Recorder.h"
#include "Recorder2.h"

#include <thread>
#include <math.h>
#include <string.h>

#define DEVICE_LEFT 17
#define DEVICE_RIGHT 18
#define DEVICE_SPEAKER 17

#define CHANNEL_LEFT 1
#define CHANNEL_RIGHT 4
#define SAMPLERATE 48000

#define NORM_MUL 32765
#define SNR 5.0

#define CHANNEL_SPEAKER 6

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
  //RecorderCX20921FW recorder_right(argv[3],argv[4],CHANNEL_RIGHT,DEVICE_RIGHT,SAMPLERATE);
  Recorder recorder_right(argv[3],argv[4],CHANNEL_RIGHT,DEVICE_RIGHT,SAMPLERATE);
  Recorder2 recorder_left(argv[3],argv[4],CHANNEL_LEFT,DEVICE_LEFT,SAMPLERATE);
  RtOutput speaker(DEVICE_SPEAKER,CHANNEL_SPEAKER,16000,48000,128,512);

  FILE *fp_c=nullptr,*fp_n=nullptr,*fp_a=nullptr;
  unsigned int nRead_c = 0,nRead_n = 0,nRead_a=0;
  unsigned int noise_start,noise_length;
  short *buf_c=nullptr,*buf_n=nullptr;
  short *buf_s=nullptr;

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
    noise_length = nRead_n / 2 ;
    
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

	/* Clipping Check*/
	/*
	for(int i=0;i<noise_length;i++){
		if(std::abs(buf_n[i])>32000)
			printf("%d %d\n",i,buf_n[i]);
	}
	*/

    //speaker_n.FullBufLoad(buf_n,noise_length);
    printf("noisy speech loaded\n");

	/* Dump Reference */
	/*
	WAV ref(2,16000);
	std::string ref_path;
	ref_path = std::string(argv[3])+"/reference/"+std::string(argv[4]);
	ref.NewFile(ref_path.c_str());
	ref.Append(buf_n,noise_length);
	ref.Finish();
	*/
	
  }

  std::thread *thread_record_right;
  std::thread *thread_record_left;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  //speaker_c.FullBufLoad(buf_c, nRead_c / 2);
  int max_len = (noise_length/2 > nRead_c/2)? noise_length/2 : nRead_c/2;

  buf_s = new short[max_len*CHANNEL_SPEAKER];
  memset(buf_s,0,sizeof(short)*max_len*CHANNEL_SPEAKER);

  /* Merge Speaker buffer */
  for(int i=0;i<nRead_c/2;i++){
    //buf_s[CHANNEL_SPEAKER*i] = buf_c[i];
    buf_s[CHANNEL_SPEAKER*i] = buf_c[i];
   // buf_s[CHANNEL_SPEAKER*i] = buf_c[i];
  }
  for(int i=0;i<noise_length/2;i++){
    buf_s[CHANNEL_SPEAKER*i+3] =buf_n[2*i];
    buf_s[CHANNEL_SPEAKER*i+5]=buf_n[2*i+1];
    //buf_s[CHANNEL_SPEAKER*i+2]=buf_n[2*i];
    //buf_s[CHANNEL_SPEAKER*i+3]=buf_n[2*i+1];
  }

  speaker.FullBufLoad(buf_s, max_len*CHANNEL_SPEAKER);

  //thread_record_right= new std::thread(&RecorderCX20921FW::Process,&recorder_right);
  thread_record_right= new std::thread(&Recorder::Process,&recorder_right);
  thread_record_left= new std::thread(&Recorder2::Process,&recorder_left);

  printf("NOTE::RECORDING STARTED\n");
  speaker.Start();
  printf("NOTE::PLAYING SOUND\n");
  speaker.Wait();

  printf("STOP RECORDING\n");
  recorder_right.Stop();
  recorder_left.Stop();

  delete[] buf_c;
  if(buf_n) delete[] buf_n;
  return 0;
}

