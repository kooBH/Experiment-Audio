2021-09-13
R705    
  
input : MEMS 16-ch

output : 1 speakers for 2 devices(clean, noise)  

target : alexa + CHiME4

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
