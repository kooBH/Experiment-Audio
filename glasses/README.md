# 안경형 MEMS 어레이  
  
  
https://kli.korean.go.kr/corpus/main/requestMain.do  
서울말 낭독체 발화 말뭉치  

2024.02.23  
안경형 마이크 어레이의 소자 성능 측정 실험.  

## 
```Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))```


```choco install ffmpeg```

## Run

+ Windows 환경 가정

1. CMake Build 수행
2. AudioProbe.exe 실행
   + 입력 장치의 ID -> main.cpp의 DEVICE_CLEAN 값으로
   + 출력 장치의 ID -> main.cpp의 DEVICE_1 값으로
3. Glasses_MEMS.exe 빌드
4. test.ps1 수정  

build의 test.ps1
```powershell
$DIR_IN = "E:\Experiment-Audio\glasses\samples"
$DIR_OUT = "E:\Experiment-Audio\glasses\output"

$Files = Get-ChildItem $DIR_IN -Filter *.wav
 
$cnt = 0
$total = $Files.Count
foreach($file in $Files){
	echo "${cnt} / ${total} : $file"
	./Glasses_MEMS.exe $DIR_OUT $file $DIR_IN/$file
	$cnt = $cnt+1
	
}
```

```DIR_IN```, ```DIR_OUT```을 설정  

5. test.ps1을 powershell에서 실행     
   + glasses.exe랑 test.ps1이랑 경로도 맞추기   

```
PS E:\Experiment-Audio\glasses\build> .\test.ps1
0 / 6 : 0000000003_0104MA1111.wav
********************
Clean speech loaded
NOTE::INITALIZED
NOTE::RECORDING STARTED
input_size :2048
NOTE::PLAYING SOUND
input_size :128
STOP RECORDING
Rt Output destructor
1 / 6 : 0000000009_0104MA1111.wav
********************
Clean speech loaded
NOTE::INITALIZED
NOTE::RECORDING STARTED
input_size :2048
NOTE::PLAYING SOUND
input_size :128
```
이런 형태의 log가 나옴. 