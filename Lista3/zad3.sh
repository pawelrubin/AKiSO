#!/bin/bash
str=$(curl -s --request GET  --url 'https://api.thecatapi.com/v1/images/search?format=json' --header 'Content-Type: application/json'  --header 'x-api-key: 82ffde73-16ba-4aee-8bcd-a3eb7a1cb791' --header 'mime_type: jpg' | jq -r '.[0].url')
echo $str
wget -q $str
img2txt -f utf8 -W 150 "${str##*/}"
curl -s -get http://api.icndb.com/jokes/random/ | jq '.value.joke' | sed 's/&quot;/\"/g'
rm "${str##*/}" 
