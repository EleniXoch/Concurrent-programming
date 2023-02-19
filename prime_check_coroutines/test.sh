#!/bin/bash
RED='\033[0;31m'
GREEN='\033[1;32m'
NC='\033[0m' # No Color

for (( i=1; i<=25; i++ ))
do
	./prime 4 <10000.txt >out"$i"
	#lines = $()
	lines=$(wc -l out"$i")
	echo $lines
	if [ "$lines" == "10000 out${i}" ]; then
	  	echo -e "${GREEN}TEST$i SUCCESS${NC}"
	else
	 	echo -e "${RED}TEST$i FAIL${NC}"
	fi
	rm out"$i"
done