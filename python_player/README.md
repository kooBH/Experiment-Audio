# Python player

파이썬 기반의 재생스크립트

+ ```python run.py``` 를 사용  
+ ```run.py``` 내의 ```dir_input```,```list_play``` 로 재생목록 관리   

동작순서
1. HostAPI : MME, Windows DirectSodund, WASAPI 같은 오디오 API 선택, WASAPI 권장
2. 출력 장치 선택
3. 재생  

```### Play ###``` 부분에서 pause나 resampling등의 관리 수행 