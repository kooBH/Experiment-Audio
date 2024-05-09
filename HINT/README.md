# 자체 HINT 실험


## HINT : Hearing In Noise Test  
잡음이 있는 상황에서 얼마나 음성을 인지할 수 있는 가에 대한 실험. 
  
  
+ 3가지 타입    
1. 측정자 전면에 clean 스피커  
2. 측정자 전면에 clean과 noise가 같이 나오는 스피커  
3. 측정자 전명에 clean 스피커, 측면에 noise 스피커  

+ 평가 방식  
clean 스피커에 나오는 음성을 측정자가 따라 말한다. 
clean의 크기는 점차 작아진다.  
측정자의 발화를 통해 측정자가 인지할 수 있는 최소의 SNR을 평가한다.   

+ 자체 시뮬레이션 시나리오   
   
clean 데이터 풀을 읽음    
noise 데이터 풀을 읽음    
clean 풀에서 점차 dB가 감소하도록 데이터를 믹싱  
noisy 상황 시에는 개별 utterance별 SNR을 측정  


