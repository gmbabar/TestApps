
CC = g++
FLAGS = -g -std=c++11 -I../../json/rapidjson/include
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = FormatMessages 

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `find . -maxdepth 1 -type f | egrep -v "Makefile|\.cpp|\.hpp|\.h|include|git" | xargs`


