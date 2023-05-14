#include "RtOutput.h"
#include "Recorder.h"

#include <random>
#include <string.h>


// Dev option
#define FLAG_NOISE 1
#define MUTE_CLEAN 0
#define USE_CLEAN2 1

// IO
#define DEVICE_1 12 // UMA8
#define DEVICE_CLEAN 4
#define DEVICE_NOISE 1
#define DEVICE_CLEAN2 5

// Data
#define SNR 5
#define ROOT "C:/20230514_R715/"
#define NAME "4spk_SNR_5dB.wav"
#define PATH_CLEAN "n_spk_2.wav"
//#define PATH_CLEAN "long.wav"
#define PATH_CLEAN2 "n_spk_2_2nd.wav"
//#define PATH_NOISE "noise_cafe.wav"
//#define PATH_NOISE "long_cafe.wav"
#define PATH_NOISE "cafe_26sec.wav"

#define NORM_MUL 32767

#define CHANNELS_1 7

#define SAMPLERATE 16000

#define NOISE_PAD 1.0


void AudioProbe();

/* 
 *	argv[1] : output_root  : /home/data/DS20924
 *	argv[2] : dir/file     :  dt05_dus_real/F10234023.wav
 *	argv[3] : UMA8
 *	argv[4] : clean_path
 *	argv[5] : noise_path
 *  argv[6] : SNR
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

  short max_n=0,max_c=0,max_c2=0;

  /* Recorder(<root_dir>,<file_name>)*/
  Recorder recorder_UMA8(ROOT,"UMA8", NAME, CHANNELS_1, DEVICE_1, SAMPLERATE);
  RtOutput speaker_c(DEVICE_CLEAN,2,SAMPLERATE,48000,128,512);
  RtOutput speaker_c2(DEVICE_CLEAN2,2,SAMPLERATE,48000,128,512);
  RtOutput speaker_n(DEVICE_NOISE,1,SAMPLERATE,48000,128,512);

  FILE *fp_c=nullptr,*fp_n=nullptr, *fp_c2=nullptr;
  unsigned int nRead_c = 0,nRead_n = 0, nRead_c2=0;
  unsigned int noise_start;
  short *buf_c=nullptr,*buf_n=nullptr,*buf_c2=nullptr;
  
  fp_c = fopen(PATH_CLEAN, "rb");
  fseek(fp_c, 0L, SEEK_END);
  nRead_c= ftell(fp_c)-44;  // 44 : WAV format head size
  fseek(fp_c, 44, SEEK_SET);

  buf_c =  new short[nRead_c/2];
  fread(buf_c,sizeof(short),nRead_c/2,fp_c);
  /* Normalize C */
  for(int i=0;i<nRead_c/2;i++)
    if(max_c < std::abs(buf_c[i]))
      max_c = abs(buf_c[i]);
  for (int i = 0; i < nRead_c / 2; i++) {
    if (MUTE_CLEAN)
      buf_c[i] = 0;
    else
      buf_c[i] = (short)(((double)(buf_c[i]) / (max_c + eps)) * NORM_MUL);
  }

  if (USE_CLEAN2) {
    fp_c2 = fopen(PATH_CLEAN2, "rb");
    fseek(fp_c2, 0L, SEEK_END);
    nRead_c2= ftell(fp_c2)-44;  // 44 : WAV format head size
    fseek(fp_c2, 44, SEEK_SET);

    buf_c2 =  new short[nRead_c2/2];
    fread(buf_c2,sizeof(short),nRead_c2/2,fp_c2);
    /* Normalize C */
    for(int i=0;i<nRead_c2/2;i++)
      if(max_c2 < std::abs(buf_c2[i]))
        max_c2 = abs(buf_c2[i]);
    for (int i = 0; i < nRead_c2 / 2; i++) {
      if (MUTE_CLEAN)
        buf_c2[i] = 0;
      else
        buf_c2[i] = (short)(((double)(buf_c2[i]) / (max_c2 + eps)) * NORM_MUL);
    }

  }

  printf("Clean speech loaded\n");

  if(FLAG_NOISE){
  	fp_n = fopen(PATH_NOISE,"rb");

	/* Set Noise Length */
    fseek(fp_n, 0L, SEEK_END);
    nRead_n= ftell(fp_n)-44;  // 44 : WAV format head size
    fseek(fp_n, 44, SEEK_SET);
    int noise_length = nRead_n / 2;

    buf_n =  new short[noise_length];
    fread(buf_n,sizeof(short),noise_length,fp_n);

    speaker_n.FullBufLoad(buf_n,noise_length);
    printf("noisy speech loaded\n");
  }

  std::thread *thread_record_1;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  thread_record_1= new std::thread(&Recorder::Process,&recorder_UMA8);
  printf("NOTE::RECORDING STARTED\n");
  if (FLAG_NOISE) {
    speaker_n.Start();
    printf("PLAYING NOISE\n");
  }

  SLEEP(300);

  speaker_c.FullBufLoad(buf_c, nRead_c / 2);
  if(USE_CLEAN2)
    speaker_c2.FullBufLoad(buf_c2, nRead_c2 / 2);
  printf("NOTE::PLAYING SOUND\n");
  speaker_c.Start();
  if(USE_CLEAN2)
    speaker_c2.Start();

  speaker_c.Wait();
  if(USE_CLEAN2)
    speaker_c2.Wait();
  if(FLAG_NOISE)
    speaker_n.Wait();

  printf("STOP RECORDING\n");
  recorder_UMA8.Stop();

  delete[] buf_c;
  if(buf_n) delete[] buf_n;
  if(buf_c2) delete[] buf_c2;
  return 0;
}

