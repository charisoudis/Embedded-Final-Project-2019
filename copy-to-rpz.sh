#!/bin/bash

clear

sshpass -p espx2019 scp ./cmake-build-release-raspberry-pi-zero/Final root@10.0.86.00:/root/Final &&
echo "Copied to 10.0.86.00"

sshpass -p espx2019 scp ./cmake-build-release-raspberry-pi-zero/Final root@10.0.90.26:/root/Final &&
echo "Copied to 10.0.90.26"


exit