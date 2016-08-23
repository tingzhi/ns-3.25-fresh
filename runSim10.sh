#!/bin/bash

for i in `seq 1 100`;
do
	echo $i
	./waf --run "test2 --outBrTimeFile=data/brTime_10_$i.txt --RngRun=$i"
done
