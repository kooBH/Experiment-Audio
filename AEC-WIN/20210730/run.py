import os,glob

root_clean = 'D:\\Experiment-Audio\\clean\\'
root_music = 'D:\\Experiment-Audio\\Noise1\\'
root_TIMIT = 'D:\\Experiment-Audio\\Noise2\\'
root_mix   = 'D:\\Experiment-Audio\\Noise12\\'

root_output = 'C:\\data'
dir_MEMS    = '20210803_MEMS'
dir_921     = '20210803_921'

list_clean = [x for x in glob.glob(os.path.join(root_clean,'*.wav'))]

print(len(list_clean))


def run(idx):
    path_clean = list_clean[idx]
    name_clean = path_clean.split('\\')[-1]
    path_noise = root_music + name_clean

    os.system('.\\build\\Release\\RECORD ' + path_clean + ' ' + path_noise + ' ' + root_output +' '+  name_clean + ' ' +dir_MEMS + '  ' + dir_921 )
    

if __name__ == '__main__' : 
    os.makedirs(root_output+'\\'+dir_MEMS,exist_ok=True)
    os.makedirs(root_output+'\\'+dir_921,exist_ok=True)

    os.system('.\\build\\Debug\\AudioProbe.exe')
    for i in range(len(list_clean)):
        run(i)