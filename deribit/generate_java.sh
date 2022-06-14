#!/bin/sh
java -Dsbe.generate.ir=true -Dsbe.target.language=java -Dsbe.target.namespace=deribit_multicast -Dsbe.output.dir=java -jar sbe-all-1.25.1.jar deribit_multicast.xml
