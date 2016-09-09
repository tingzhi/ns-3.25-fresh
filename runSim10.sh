#!/bin/bash

for i in `seq 1 10`;
do
	echo $i
	./waf --run "test2 --outBrTimeFile=data/brTime_10_$i.txt --outLifeSpanFile=data/lifeSpan_10.txt --outOverheadFile=data/overhead_10.txt --outConsumedEnergyFile=data/energy_10.txt --RngRun=$i"
done
