#!/bin/bash

echo ./producer "sv-awoh-kafka1.na.bluefirecap.net:9092,sv-awoh-kafka2.na.bluefirecap.net:9092" v1.app.test
./producer "sv-awoh-kafka1.na.bluefirecap.net:9092,sv-awoh-kafka2.na.bluefirecap.net:9092" v1.app.test

