#!/bin/bash

(
	printf 'NAME\tPID\tSTATE\tFILES\n'

	for i in $(sudo ls /proc  | grep '[0-9]' ); do
		if [[ -d "$(echo /proc/$i)" ]]; then
			
			name=$(echo $(awk 'FNR == 1 ' /proc/$i/status | cut -d ":" -f2)  )
			pid=$(echo $(awk 'FNR == 6 ' /proc/$i/status | cut -d ":" -f2) )
			state=$(echo $(awk 'FNR == 3 ' /proc/$i/status | cut -d ":" -f2) )
			files=$(echo $(ls -l -s /proc/$i/fd | wc -l) )

			printf '%s\t%s\t%s\t%s\n' "$name" "$pid" "$state" "$files"
		fi
	done
) | column -t -s $'\t' 

