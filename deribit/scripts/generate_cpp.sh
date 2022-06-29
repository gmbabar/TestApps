#!/bin/sh

## NOTE:
# - Install java / jdk
# - Compile sbe by running 'gradlew' in sbe/sbe-1.25.3

SBE_TOOL=../../sbe/sbe-1.25.3/sbe-all/build/libs/sbe-all-1.25.3.jar
OUT_DIR=../cpp
SCHEMA=../schema/deribit_multicast.xml
java -Dsbe.generate.ir=true -Dsbe.target.language=cpp -Dsbe.target.namespace=deribit_multicast -Dsbe.output.dir=${OUT_DIR} -jar ${SBE_TOOL} ${SCHEMA}
