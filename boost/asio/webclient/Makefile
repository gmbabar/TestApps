
CC = g++
FLAGS = -g -std=c++11 -I. -I../../../json/rapidjson/include/
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = symbology web_client simple 

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

# Inactive.
ServerSimulator: ServerSimulator.cpp
	g++ $(FLAGS) -o $@  $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|common" | xargs`

