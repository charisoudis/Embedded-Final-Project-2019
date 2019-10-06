#!/bin/bash

clear
JSONSPATH="$HOME/Desktop/HY3604/Project 2 - Final/report/WebReport/Sessions/"
PATH0001="${JSONSPATH}0001/"
PATH9026="${JSONSPATH}9026/"
PATH8600="${JSONSPATH}8600/"

# Copy from device
## get index
JSONFNAME="${RANDOM}.json" && touch "${PATH0001}${JSONFNAME}" && COUNT=$(find "${PATH0001}" -type f -name '*.json' | wc -l) && rm "${PATH0001}${JSONFNAME}" &&
mv ./cmake-build-debug/session1.json "${PATH0001}session${COUNT}.json" &&
echo "Copied from Laptop"

# Copy from 1st device to JSONs directory
JSONFNAME="${RANDOM}.json" && touch "${PATH9026}${JSONFNAME}" && COUNT=$(find "${PATH9026}" -type f -name '*.json' | wc -l) && rm "${PATH9026}${JSONFNAME}" &&
sshpass -p espx2019 scp root@10.0.90.26:/root/session1.json "${PATH9026}session${COUNT}.json" &&
echo "Copied from 10.0.90.26"

# Copy from 2nd device to JSONs directory
sshpass -p espx2019 scp root@10.0.86.00:/root/session1.json "${PATH8600}session${COUNT}.json" &&
echo "Copied from 10.0.86.00"

exit