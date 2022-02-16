# Note: -lboost_system to be removed on linux hosts
all:
	g++ -std=c++11 -o buffers buffers.cpp  -lpthread
	g++ -std=c++11 -o multibuffer multibuffer.cpp  -lpthread
	g++ -std=c++11 -o flatbuffer flatbuffer.cpp  -lpthread
	g++ -std=c++11 -o flatstaticbuffer flatstaticbuffer.cpp  -lpthread

clean:
	rm -f multibuffer flatstaticbuffer flatbuffer buffers

