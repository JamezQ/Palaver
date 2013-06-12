#!/bin/bash

read PID_OF_OSD < osd.pid
kill $PID_OF_OSD
read PID_OF_IND < indicator.pid
kill $PID_OF_IND
rm osd.pid
rm indicator.pid

./Microphone/indicator_server.py 2>/dev/null & 
echo $! > "indicator.pid"
./Microphone/osd_server.py 2>/dev/null & 
echo $! > "osd.pid"
