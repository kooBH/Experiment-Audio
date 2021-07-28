#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>

#define DEVICE_1 2 // MEMS
#define DEVICE_2 20  // DS20924

#define DEVICE_CLEAN 21
#define DEVICE_NOISE 0

#define NORM_MUL 32767

#define CHANNELS_1 12
#define CHANNELS_2 2

#define SAMPLERATE 16000
#define SAMPLERATE_2 48000

#define NOISE_PAD 1.0

#define SNR 0.0

void AudioProbe();

/* 
 *	argv[1] : output_root  : /home/data/DS20924
 *	argv[2] : dir/file     :  dt05_dus_real/F10234023.wav
 *	argv[3] : MEMS
 *	argv[4] : DS20924
 *	argv[5] : clean_path
 *	argv[6] : noise_path
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

  bool flag_noise=true;

  /* Recorder(<root_dir>,<file_name>)*/
  Recorder recorder_MEMS(argv[1],argv[3],argv[2],CHANNELS_1,DEVICE_1,SAMPLERATE);
  Recorder recorder_924(argv[1],argv[4],argv[2],CHANNELS_2,DEVICE_2,SAMPLERATE);
  RtOutput speaker_c(DEVICE_CLEAN,1,SAMPLERATE,48000,128,512);
  RtOutput speaker_n(DEVICE_NOISE,1,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr,*fp_n=nullptr,*fp_a=nullptr;
  unsigned int nRead_c = 0,nRead_n = 0,nRead_a=0;
  unsigned int noise_start;
  short *buf_c=nullptr,*buf_n=nullptr,*buf_a;

  fp_a = fopen("Alexa_16k.wav","rb");
  
  fseek(fp_a,0L,SEEK_END);
  nRead_a =	ftell(fp_a)-44;
  fseek(fp_a,44,SEEK_SET);

  buf_a =  new short[nRead_a/2];
  fread(buf_a,sizeof(short),nRead_a/2,fp_a);
  /* Normalize C */
  for(int i=0;i<nRead_a/2;i++)
	  if(max_a < std::abs(buf_a[i]))
		  max_a = abs(buf_a[i]);
  for(int i=0;i<nRead_a/2;i++)
	  buf_a[i] = (short)( ((double)(buf_a[i])/(max_a+eps))*NORM_MUL );

  speaker_c.FullBufLoad(buf_a,nRead_a/2);

  fp_c = fopen(argv[5], "rb");

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

  if(flag_noise){
	fp_n = fopen(argv[6],"rb");

	/* Set Noise Length */
    fseek(fp_n, 0L, SEEK_END);
    nRead_n= ftell(fp_n)-44;  // 44 : WAV format head size
    fseek(fp_n, 44, SEEK_SET);
	int noise_length = nRead_c/2 + nRead_a/2 + int(SAMPLERATE*NOISE_PAD);

	 /* Noise Random Sampling */	
	std::random_device rd;
	std::mt19937 gen(rd());

	// Fix seed
	gen.seed(nRead_c);

	std::uniform_int_distribution<int> rand_noise(0,nRead_n/2 - noise_length			);

	int noise_start = rand_noise(gen)*2; // 2btyes sample 

	// random sampling 
	fseek(fp_n,noise_start,SEEK_SET);
	/*
	printf("nRead_n      : %d\n",nRead_n);
	printf("nRead_c      : %d\n",nRead_c);
	printf("nRead_a      : %d\n",nRead_a);
	printf("padding      : %d\n",int(SAMPLERATE*NOISE_PAD));
	printf("noise_length : %d\n",noise_length);
	printf("noise_start  : %d\n",noise_start);
	*/

	/* Cal Energy */
	  for(int i=0; i <nRead_c/2; i++)
		  energy_clean += buf_c[i]*buf_c[i];
	  mean_energy_clean = energy_clean/(nRead_c/2);

    buf_n =  new short[noise_length];
    fread(buf_n,sizeof(short),noise_length,fp_n);

    /* Normalize N */
	/*
    for(int i=0;i<noise_length;i++)
      if(max_n < std::abs(buf_n[i]))
        max_n = abs(buf_n[i]);
	*/

    /* E noise */
    for(int i=0;i<noise_length;i++){
      energy_noise_1 += buf_n[i]*buf_n[i];
	}
    mean_energy_noise_1 = energy_noise_1/(noise_length);

    /* SNR */
    energy_normal_1 = std::sqrt(mean_energy_clean)/std::sqrt(mean_energy_noise_1);
    double SNRweight_1 = energy_normal_1/(std::sqrt(std::pow(10,SNR/10)));

    for(int i=0;i<noise_length;i++){
      buf_n[i]*=SNRweight_1;
	}

    speaker_n.FullBufLoad(buf_n,noise_length);
    printf("noisy speech loaded\n");
  }

  std::thread *thread_record_1;
  std::thread *thread_record_2;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  thread_record_1= new std::thread(&Recorder::Process,&recorder_MEMS);
  thread_record_2= new std::thread(&Recorder::Process,&recorder_924);

  printf("NOTE::RECORDING STARTED\n");
  if(flag_noise)
    speaker_n.Start();

  SLEEP(300);

  speaker_c.Start();
  speaker_c.Wait();
  speaker_c.FullBufLoad(buf_c, nRead_c / 2);
  printf("NOTE::PLAYING SOUND\n");
  speaker_c.Start();

  speaker_c.Wait();
  if(flag_noise)
    speaker_n.Wait();

  // 1-sec delay
  if(!flag_noise)
	SLEEP(NOISE_PAD*1000);

  printf("STOP RECORDING\n");
  recorder_MEMS.Stop();
  recorder_924.Stop();

  delete[] buf_c;
  if(buf_n) delete[] buf_n;
  return 0;
}

