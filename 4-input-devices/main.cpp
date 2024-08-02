#include "Recorder.h"

#include <chrono>
#include <random>
#include <string.h>

#define DEVICE_1 4
#define DEVICE_CLEAN 3



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

  int device1 = 0;
  int device2 = 0;
  int device3 = 0;
  int device4 = 0;

  int in_channels_device1 = 0;
  int in_channels_device2 = 0;
  int in_channels_device3 = 0;
  int in_channels_device4 = 0;

  int samplerate = 16000;

  std::string dir_1 = "output_of_devices 1";
  std::string dir_2 = "output_of_devices 1";
  std::string dir_3 = "output_of_devices 1";
  std::string dir_4 = "output_of_devices 1";

  /* Recorder(<root_dir>,<file_name>) -> <root_dir>/<file_name>.wav*/
  Recorder recorder_device_1(dir_1,argv[1],in_channels_device1, device1, samplerate);
  Recorder recorder_device_2(dir_2,argv[1],in_channels_device2, device2, samplerate);
  Recorder recorder_device_3(dir_3,argv[1],in_channels_device3, device3, samplerate);
  Recorder recorder_device_4(dir_4,argv[1],in_channels_device4, device4, samplerate);

  std::thread *thread_record_1;
  std::thread *thread_record_2;
  std::thread *thread_record_3;
  std::thread *thread_record_4;
  printf("NOTE::INITALIZED\n");

  /* Routine */
  thread_record_1 = new std::thread(&Recorder::Process,&recorder_device_1);
  thread_record_2 = new std::thread(&Recorder::Process,&recorder_device_2);
  thread_record_3 = new std::thread(&Recorder::Process,&recorder_device_3);
  thread_record_4 = new std::thread(&Recorder::Process,&recorder_device_4);

  printf("NOTE::RECORDING STARTED\n");

  auto start = std::chrono::high_resolution_clock::now();
  auto end = start + std::chrono::minutes(10);
  while (std::chrono::high_resolution_clock::now() < end) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
    std::cout << "Elapsed time: " << elapsed << " seconds" << std::endl;
  }

  recorder_device_1.Stop();
  recorder_device_2.Stop();
  recorder_device_3.Stop();
  recorder_device_4.Stop();
  return 0;
}

