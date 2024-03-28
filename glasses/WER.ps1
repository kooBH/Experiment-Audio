$DIR_GT = "C:\workspace\Experiment-Audio\glasses\label_t_s"
$DIR_LOG = "C:\workspace\Experiment-Audio\glasses\log"


$DIR_AUD = "C:\workspace\Experiment-Audio\glasses\20240306_A_1m_60dB"
python WER.py $DIR_AUD $DIR_GT $DIR_LOG 

$DIR_AUD = "C:\workspace\Experiment-Audio\glasses\20240307_F_1m_60dB"
python WER.py $DIR_AUD $DIR_GT $DIR_LOG 

$DIR_AUD = "C:\workspace\Experiment-Audio\glasses\20240308_D_1m_60dB"
python WER.py $DIR_AUD $DIR_GT $DIR_LOG 


$DIR_AUD = "C:\workspace\Experiment-Audio\glasses\20240324_D_2m_60dBOn1m"
python WER.py $DIR_AUD $DIR_GT $DIR_LOG 

$DIR_AUD = "C:\workspace\Experiment-Audio\glasses\20240326_A_2m_60dBOn1m"
python WER.py $DIR_AUD $DIR_GT $DIR_LOG 


$DIR_AUD = "C:\workspace\Experiment-Audio\glasses\20240326_F_2m_60dBOn1m"
python WER.py $DIR_AUD $DIR_GT $DIR_LOG 

