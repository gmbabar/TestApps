# Note: -lboost_system to be removed on linux hosts
all:
	#g++ -std=c++11 -o test test.cpp -lboost_iostreams
	#g++ -std=c++11 test-decompress2.cpp -o t2 -lpthread -lboost_iostreams -lboost_unit_test_framework -lz
	g++ -std=c++11 TestDecompress.cpp -o TestDecompress -lpthread -lboost_iostreams -lboost_unit_test_framework -lz

clean:
	rm -f test TestDecompress
