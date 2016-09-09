#!/bin/bash

cd data

for i in `seq 10 10 200`;
do
	#echo brTime_$i_*.txt
	echo $i
	mv brTime_"$i"_*.txt brTime_"$i"	
done

