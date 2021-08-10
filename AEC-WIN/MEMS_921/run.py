import os,glob
import time

root_clean = 'D:\\Experiment-Audio\\clean\\'
root_music = 'D:\\Experiment-Audio\\Noise1\\'
root_TIMIT = 'D:\\Experiment-Audio\\Noise2\\'
root_mix   = 'D:\\Experiment-Audio\\Noise12\\'

root_output = 'C:\\data'
dir_MEMS    = '20210807_MEMS'
dir_921     = '20210807_921'

list_clean = [x for x in glob.glob(os.path.join(root_clean,'*.wav'))]

print(len(list_clean))


def run(idx):

    path_clean = list_clean[idx]
    name_clean = path_clean.split('\\')[-1]
    path_noise = root_music + name_clean
    cmd = '.\\build\\Release\\RECORD ' + path_clean + ' ' + path_noise + ' ' + root_output +' '+  name_clean + ' ' +dir_MEMS + '  ' + dir_921 
    print('cmd::' + cmd)
    os.system(cmd)
    

if __name__ == '__main__' : 
    os.makedirs(root_output+'\\'+dir_MEMS,exist_ok=True)
    os.makedirs(root_output+'\\'+dir_921,exist_ok=True)

    time.sleep(10)

    os.system('.\\build\\Debug\\AudioProbe.exe')
    for i in range(len(list_clean)):
        run(i)

        # for every 100 inputs
        time.sleep(0.5)
        if i % 100 == 99 :
            time.sleep(30)
    #for i in range(10):
    #    run(i)
    #while True :
    #    run(10)
