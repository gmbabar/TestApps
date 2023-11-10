#!/bin/bash

while true
do
	date
	./PriceFeedTester.py || { echo; echo 'SNAPSHOT TESTER ENCOUNTERED RUNTIME ERROR' ; exit 1; }
	date
	echo "Press [CTRL+C] to stop.. <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
	sleep 10
done

