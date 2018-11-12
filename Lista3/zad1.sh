#/bin/bash

if (( $# != 3 )); then
	echo "wrong number of parameters"
	echo "\$1 - width, \$2 - height, \$3 bar width"
	exit
fi

#-------------VARIABLES---------------
height=$2
barWidth=$3
width=$(echo "$1 - ($1%($barWidth+1)) + 1" | bc)
widthGraph=$(echo "$1/($barWidth+1)" | bc)

downloadColor=$(tput setaf 6)
uploadColor=$(tput setaf 5)
normal=$(tput sgr0)

downloadArray=()
uploadArray=()
downloadHeightArray=()
uploadHeightArray=()

for (( i=0; i<$widthGraph; i++)); do
	downloadArray+=(0)
	uploadArray+=(0)
	downloadHeightArray+=(0)
	uploadHeightArray+=(0)
done

globalUpMax=0
globalDownMax=0
maxDownload=${downloadArray[0]}
maxUpload=${uploadArray[0]}

downloadScale=0
uploadScale=0

numOfDev=$(ls /sys/class/net | wc -w)

download=0
upload=0
avgDownload=0
avgUpload=0
counterDownload=0
counterUpload=0
uptime=""
battery=""
loadAvg=""


#-------------------------------------
trap ctrl_c INT
stty -ctlecho

# this is executed after pressing ctr+c
function ctrl_c() {
	temp=$(echo "16 + $height * 2" | bc)
	for (( i=0; i<$temp; i++ )); do
		echo ""
	done

	tput cnorm
	stty ctlecho
	exit
}

# calculating current download and upload speed
function calculateSpeed {
	rec1=0
	t1=0
	for (( i=0; i<$numOfDev; i++ )); do
		index=$(($i + 3))
		tempRec=$(awk -v i=$index 'FNR == i {print $2}' /proc/net/dev)
		tempT=$(awk -v i=$index 'FNR == i {print $10}' /proc/net/dev)
		rec1=$(($rec1 + $tempRec))
		t1=$(($t1+$tempT))
	done

	sleep "1"

	rec2=0
	t2=0
    for (( i=0; i<$numOfDev; i++ )); do
		index=$(($i + 3))
		tempRec=$(awk -v i=$index 'FNR == i {print $2}' /proc/net/dev)
		tempT=$(awk -v i=$index 'FNR == i {print $10}' /proc/net/dev)
		rec2=$(($rec2 + $tempRec))
		t2=$(($t2+$tempT))
	done
	
	download=$(($rec2-$rec1))
	upload=$(($t2-$t1))
}

# function returns new average value 
function calculateNewAvg {
	local avg=$1	
	local value=$2
	local counter=$3

	newAvg=$(echo "$avg + ($value - $avg)/$counter" | bc)

	echo "$newAvg"
}

# calculating average download and upload speed
function calculateAvgSpeed {
	if (( $download > 0 )); then
		counterDownload=$(($counterDownload+1))
		avgDownload=$(calculateNewAvg "$avgDownload" "$download" "$counterDownload")
	fi

	if (( $upload > 0 )); then
		counterUpload=$(($counterUpload+1))
		avgUpload=$(calculateNewAvg "$avgUpload" "$upload" "$counterUpload")
	fi
}

# getting info about uptime, battery and loadavg
function uptimeBatteryLoadAvg {
	uptime=$(awk '{print $1}' /proc/uptime)
	if [ -f /sys/class/power_supply/BAT0/uevent ]; then
		battery=$(cat /sys/class/power_supply/BAT0/uevent | grep -o "POWER_SUPPLY_CAPACITY=.*" | cut -d"=" -f2)
	fi
	loadAvg=$(cat /proc/loadavg)
}

# recursive function for changing units
function format {
	units=(B KB MB GB)
	i=$2
	if (( ${1%.*} < 1000 )); then # ${1%.*} - erasing decimal fraction
		echo "$1 ${units[$i]}"
	else 
		format "$(echo "scale=2;$1/1000" | bc)" "$(($2+1))"
	fi
}

# printing info
function printOnScreen {
	sec=${uptime%.*}
	min=$(echo "($sec/60.0)%60" | bc)
	hours=$(echo "($sec/60/60)%24" | bc)
	days=$(echo "$sec/60/60/24" | bc)
	sec=$(echo "${uptime%.*}%60" | bc)

	echo -n -e "speed:\n"
	echo -n -e ",present: \u21D3 $(format $download 0)/s \u21D1 $(format $upload 0)/s                            \n"
	echo -n -e ",average: \u21D3 $(format $avgDownload 0)/s \u21D1 $(format $avgUpload 0)/s                      \n"
	echo -n -e ",maximum: \u21D3 $(format $globalDownMax 0)/s \u21D1 $(format $globalUpMax 0)/s                  \n"
	echo -n -e "uptime:\n" 
	echo -n -e ",$days d $hours h $min min $sec sec      \n"
	echo -n -e "battery:\n"
	echo -n -e ",$battery %      \n"
	echo -n -e "loadavg:\n"
	echo -n -e ",$loadAvg          \n "
}

# $1 - array, $2 - value to append
function arrayShiftAppend {
	local -n arr=$1
	local arrSize=${#arr[@]}

	for (( i=0; i<$arrSize; i++ )); do
        arr[$i]=${arr[(($i+1))]}
    done
    
	arr[-1]=$2
}

function calculateMax {
	maxDownload=${downloadArray[0]}
	maxUpload=${uploadArray[0]}

	for (( i=1; i<${#downloadArray[@]}; i++ )); do
		if (( ${downloadArray[$i]} > $maxDownload )); then
            maxDownload=${downloadArray[$i]}
        fi
		
		if (( ${uploadArray[$i]} > $maxUpload )); then
            maxUpload=${uploadArray[$i]}
        fi
	
		if (( $maxDownload > $globalDownMax )); then
			globalDownMax=$maxDownload
		fi

		if (( $maxUpload > $globalUpMax )); then
			globalUpMax=$maxUpload
		fi
    done
}

# calculating scale, $1 - maximum value
function calculateScale {
	echo $(echo "$1/$height" | bc)
}

# calculate height array, $1 - value array, $2 - scale, $3 - height array
function calculateHeight {
	local -n arr=$1
	local -n heightArray=$2
	local scale=$3
	local arrSize=${#arr[@]}

	for (( i=0; i<$arrSize; i++ )); do
		if (( $scale == 0 )); then
			heightArray[$i]=0
		else
			heightArray[$i]=$(echo "${arr[$i]}/$scale" | bc) 
		fi
	done 
}

#print graph $1 - height array, $2 - scale, $3 - color
function drawGraph {
	local -n heightArray=$1
	local scale=$2
	local color=$3
	
	echo -n -e ",\u2554"
	for (( i=1; i<=$width; i++ )); do
		echo -n -e "\u2550"
	done
	echo -n -e "\u2557      \n"


	#wlasciwy wykresik
	for (( i=1; i<=$height; i++ )); do
		echo -n -e "$(echo $(format $(echo "$scale * ($height-$i+1)" | bc) '0')/s) ,\u2551"
		
		for (( j=1; j<=$widthGraph; j++ )); do
			if (( $(($height-$i)) < ${heightArray[$(($j-1))]} )); then
				bar=""
				for (( k=0; k<$barWidth; k++ )); do 
					bar+="\u2588"
				done
				echo -n -e " ${color}$bar${normal}"
			else
				echo -n " "
				for (( k=0; k<$barWidth; k++ )); do 
					echo -n " "
				done
			fi
		done
		
		echo -n -e " \u2551       \n"
	done
	#--------------------

	echo -n -e ",\u255A"
	for (( i=1; i<=$width; i++ )); do
		echo -n -e "\u2550"
	done
	echo -n -e "\u255D       \n"
}

clear
tput civis

#main loop
while(true); do
	tput cup 0 0
	calculateSpeed
	calculateAvgSpeed
	uptimeBatteryLoadAvg

	arrayShiftAppend downloadArray $download
	arrayShiftAppend uploadArray $upload

    calculateMax
	downloadScale=$(calculateScale $maxDownload)
	uploadScale=$(calculateScale $maxUpload)

	calculateHeight downloadArray downloadHeightArray $downloadScale
	calculateHeight uploadArray uploadHeightArray $uploadScale
	
	(
	printOnScreen
	drawGraph downloadHeightArray $downloadScale $downloadColor
	drawGraph uploadHeightArray $uploadScale $uploadColor
	) | column -t -s $','
done