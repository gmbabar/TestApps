#!/bin/sh

#java -Dsbe.generate.ir=true -Dsbe.target.language=cpp -Dsbe.target.namespace=deribit_multicast -Dsbe.output.dir=cpp -jar sbe-all-1.25.1.jar deribit_multicast.xml

SBE_TOOL=../sbe/sbe-1.25.3/sbe-all/build/libs/sbe-all-1.25.3.jar
java -Dsbe.generate.ir=true -Dsbe.target.language=cpp -Dsbe.target.namespace=deribit_multicast -Dsbe.output.dir=cpp -jar ${SBE_TOOL} deribit_multicast.xml
