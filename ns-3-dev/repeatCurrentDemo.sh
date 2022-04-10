#!/bin/bash
for delay0 in {1..1000}
do
  for delay1 in {1..1000}
  do
    ./waf --run "currentDemo --d0=$delay0 --d1=$delay1" 
  done
done

