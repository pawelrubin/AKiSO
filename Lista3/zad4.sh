#!/bin/bash

if (( $# != 2 )); then
	echo "wrong number of parameters"
	echo "\$1 - url, \$2 - time interval"
	exit
fi

url=$1
interval=$2

read -p "Directory name: " dirName

mkdir -p $dirName
cd $dirName

lynx -dump $url > site

git init
git add site

while(sleep $interval); do
	lynx -dump $url > tmp

	if [ -z "$( diff -q site tmp)" ]; then
		echo "brak zmian"
   	else
		git commit -am "zmiana" 
		echo "zmiana"
		zenity --info --text "$(echo -e "Zmiana na stronie\n$url")"
	fi

	mv tmp site
done