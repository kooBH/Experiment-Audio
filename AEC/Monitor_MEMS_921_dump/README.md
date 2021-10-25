2021-10-31  
R705A  
  
input : MEMS 10-ch

output : 1 speakers(monitor) for 2 devices(clean, noise)

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
