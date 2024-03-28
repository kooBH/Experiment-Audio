import whisper
import librosa as rs
import argparse
import metric
import os, glob
import pickle
from tqdm import tqdm


if __name__ == '__main__' : 

    parser = argparse.ArgumentParser()
    parser.add_argument("dir_input")
    parser.add_argument("dir_GT")
    parser.add_argument("dir_log")
    args = parser.parse_args()
    
    dir_input = args.dir_input
    dir_log = args.dir_log
    dir_GT = args.dir_GT
    
    #dir_GT = "label_t_s"
    whisper_size = "large"
    
    list_data = glob.glob(os.path.join(dir_input,"*.wav"))    
    
    model = whisper.load_model(whisper_size)
    print("Whisper : ",whisper_size)
    
    total_cer = 0
    
    if dir_input[-1] == "/" :
        dir_input = dir_input[:-1]
    if dir_input[-1] == "\\" :
        dir_input = dir_input[:-1]    
    nameout = os.path.basename(dir_input)
    print(nameout)    
    with open(os.path.join(dir_log,nameout+".csv"),"w", encoding='UTF-8-sig') as f_out :
        f_out.write("Index, Filename, CER, estim, GT\n")
    
        for idx,path in tqdm(enumerate(list_data),total=len(list_data)):
            # path : fv01_t01_s01.wav
            # label : label_t_s/t01_s01.txt
            
            base_name = os.path.basename(path)
            file_name = base_name.split(".")[0]
            tokens = file_name.split("_")
            
            label_name = tokens[1] + "_" + tokens[2] + ".txt"
            path_label = os.path.join(dir_GT,label_name)
            
            with open(path_label,encoding='UTF-8-sig') as f_gt:
                gt = f_gt.readlines()[0]
            
            # condition_on_previous_text
            result = model.transcribe(path,
            temperature=0,
            condition_on_previous_text=False
            )
            
            CER = metric.CharacterErrorRate()
            dist,length = CER.metric(gt, result["text"])
            cer = dist/length
            
            f_out.write("{},{},{},{},{}\n".format(idx,file_name, cer,result["text"],gt))
            total_cer += cer

        final_cer = total_cer / len(list_data)
        
        f_out.write("-1,AVG,{}".format(final_cer))
    print("CER : ", final_cer)
    