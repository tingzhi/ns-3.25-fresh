#!/bin/bash

for j in `seq 10 10 200`;
do
	for i in `seq 121 140`;
	do
		#echo "i=$i j=$j"
		./waf --run "test2 --outBrTimeFile=data/brTime_"$j"_"$i".txt --RngRun=$i --numNodes=$j"
	done
done
