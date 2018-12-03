#!/bin/bash

for file in ./*; do
	if [[ -f $file ]]; then 
		mv -i "$file" "${file,,}"
	fi
done
