#!/bin/bash

clear
#sleep 1s
JSONSSPATH="/home/argiris/Desktop/HY3604/Project 2 - Final/report/Timeline/Sessions/"

# Empty JSONs directory
rm "${JSONSSPATH:?}session1.json" &&
rm "${JSONSSPATH:?}session2.json" &&
rm "${JSONSSPATH:?}session3.json" &&
echo "Emptied JSONs directory"

# Copy from device
mv ./cmake-build-debug/session1.json "${JSONSSPATH:?}session1.json" &&
echo "Copied from Laptop"

# Copy from 1st device to JSONs directory
sshpass -p espx2019 scp root@10.0.90.26:/root/session1.json "${JSONSSPATH:?}session2.json" &&
echo "Copied from 10.0.90.26"

# Copy from 2nd device to JSONs directory
sshpass -p espx2019 scp root@10.0.86.00:/root/session1.json "${JSONSSPATH:?}session3.json" &&
echo "Copied from 10.0.86.00"

exit