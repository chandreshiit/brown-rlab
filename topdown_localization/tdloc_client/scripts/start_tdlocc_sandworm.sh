#!/bin/bash
killall tdlocc
sleep 3
./tdlocc -c 0 -x 1.315 -y 1.625 -v 1.69 -w 2.17 -h sandworm -p 8855 &
./tdlocc -c 1 -x 1.315 -y 0.875 -v 1.69 -w 2.17 -h sandworm -p 8855 &

