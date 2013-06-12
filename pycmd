#!/bin/bash

# right now it uses moving of files, but we may want to communicate with the
# python script another way.


function error() {
    echo "Accepted modes:"
    echo "wait"
    echo "done"
    echo "stop"
    echo "record"
    echo "result"
}

if [ -z "$1" ];then
    error
fi

if ps auxww |grep -v grep |grep osd_server > /dev/null 2>&1;then
    : # we are good, the server is there
else # otherwise start it
    ./Microphone/osd_server.py 2>/dev/null & 
	echo $! > "osd.pid"
fi  
if ps auxww |grep -v grep |grep indicator_server.py > /dev/null 2>&1;then
    : # we are good, the server is there
else # otherwise start it
    ./Microphone/indicator_server.py 2>/dev/null & 
    echo $! > "indicator.pid"
fi    
# This should be run from the base dir.
cd Microphone

case "$1" in
    wait)
	rm pycmd_*
	touch pycmd_wait
	;;
    done)
	rm pycmd_*
	touch pycmd_done
	;;
    stop)
	rm pycmd_*
	touch pycmd_stop
	;;
    record)
	rm pycmd_*
	touch pycmd_record
	;;
    result)
	rm pycmd_*
	touch pycmd_result
	;;
    *)
	error
	;;
esac

