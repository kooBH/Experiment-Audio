$DIR_IN = "E:\Experiment-Audio\glasses\samples"
$DIR_OUT = "E:\Experiment-Audio\glasses\output"

$Files = Get-ChildItem $DIR_IN -Filter *.wav
 
$cnt = 0
$total = $Files.Count
foreach($file in $Files){
	echo "${cnt} / ${total} : $file"
	./Glasses_MEMS.exe $DIR_OUT $file $DIR_IN/$file
	$cnt = $cnt+1
	
}
