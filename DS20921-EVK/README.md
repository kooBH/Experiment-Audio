# Experiment-DS20921-EVK
Codes for Experiments on DS20921-EVK    

+ [document](https://www.synaptics.com/partners/amazon/ds20921)  
+ [guide](https://github.com/conexant/avs-device-sdk/wiki/2-Mic-Development-Kit-User-Guide)  
  
# Senario  
1. target speech + 2-channel noise 
  
2. target speech + 2-channel(noise speech + noise)

For each senario noise must be choosen randomly.    
To perform experiments, the seed must be fixed for every experiment.   

Going to use Studio capture audio iterface, so the output for the speaker will be 3-channel.    

## TEMP
ismir04_genre : 장르별로 최소 갯수 기준으로 가운데서 샘플링.     
TIMIT : 짧으니까 concat해서 샘플링 (1초 이후)    
1초 CHiME 1초   
  
+ ismir04 only (2ch)    
   
+ ismir04 1ch + TIMIT    
  
+ TIMIT only (1ch * 2)      
  
SNR 0.0    
  
arg : CHiME4, N1, N2    

2-point noise    
 
## NUM OF ISMIR04
classical: 319
electronic: 115
jazz: 26
metal: 29
pop: 5
punk: 16
rock: 96
world: 123 


