
CC = g++

FLAGS = -g -std=c++11 -I./  -I../cpp/

## Nebula headers
#FLAGS += -I/tmp/gbabar/dist/nebula/include/

LIBS = -lboost_system -lpthread 

objects = decoder

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

