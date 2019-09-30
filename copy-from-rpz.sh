#!/bin/bash

clear
sleep 5s

# Copy from device
mv ./cmake-build-debug/session1.json ./cmake-build-debug/JSONs &&
echo "Copied from Laptop"

# Copy from 1st device to JSONs directory
shpass -p espx2019 scp root@10.0.90.26:/root/session1.json ./cmake-build-debug/JSONs/session2.json &&
echo "Copied from 10.0.90.26"

# Copy from 2nd device to JSONs directory
shpass -p espx2019 scp root@10.0.86.00:/root/session1.json ./cmake-build-debug/JSONs/session3.json &&
echo "Copied from 10.0.86.00"