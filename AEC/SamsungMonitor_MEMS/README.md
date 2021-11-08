2021-11-08
R705A  
  
input : MEMS 10-ch

output : 1 speakers(monitor) for 2 devices(clean, noise)

monitor : Samsung [LF24T650FYKXKR](https://smartstore.naver.com/hugegaram/products/5613921362?NaPm=ct%3Dkvqk66ak%7Cci%3Dshopn%7Ctr%3Ddana%7Chk%3Dd097e6c31f7f046b8ec286beb19be9b878c0e028%7Ctrx%3Dundefined) 

target : alexa + CHiME4(1ch)  
  
noise : ismir04(2ch) + CHiME noise(1ch)  
  
dump   : data from powered speaker port of DS0921   

+  Error of DS20921   
Before We thaught malfunction of DS20921 is caused by long non-stopping usage.   
But it turns out it isn't.   
For now, the power of reference signal gets suspicion.   
Therefore we reduce maximux of reference as 0.7 of usual.    

+ NOTE  
  - no SNR control in code.    
  - normalizing clean, noise and Scaling  
  - Longer Noise  
  - Using 1 output device (studio-capture)
  - Studio capture input in linux must be 48kHz  
