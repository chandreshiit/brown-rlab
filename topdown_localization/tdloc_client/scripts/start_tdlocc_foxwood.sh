#!/bin/bash
killall tdlocc
sleep 3
./tdlocc -c 0 -x 2.875 -y 1.725 -v 1.69 -w 2.17 -h sandworm -p 8855 &
./tdlocc -c 1 -x 2.875 -y 0.855 -v 1.69 -w 2.17 -h sandworm -p 8855 &

