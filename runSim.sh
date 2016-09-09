#!/bin/bash

for j in `seq 10 10 200`;
do
	#echo $j
	for i in `seq 1 100`;
	do
		#echo "i=$i j=$j"
		./waf --run "test2 --outBrTimeFile=data/brTime_"$j"_"$i".txt --outLifeSpanFile=data/lifeSpan_"$j".txt --outOverheadFile=data/overhead_"$j".txt --outConsumedEnergyFile=data/energy_"$j".txt --RngRun=$i --numNodes=$j"
	done
done
