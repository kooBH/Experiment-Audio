$DIR_IN = "C:\workspace\Experiment-Audio\glasses\samples"
$DIR_OUT = "C:\workspace\Experiment-Audio\glasses\output"

$Files = Get-ChildItem $DIR_IN -Filter *.wav
 
Start-Sleep -Seconds 60

$cnt = 0
$total = $Files.Count
foreach($file in $Files){
	echo "${cnt} / ${total} : $file"
	./Release/Glasses_MEMS.exe $DIR_OUT $file $DIR_IN/$file
	$cnt = $cnt+1
	
}
