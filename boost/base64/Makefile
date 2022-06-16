
CC = g++

FLAGS = -g -std=c++11 -I./ 

## Nebula headers
#FLAGS += -I/tmp/gbabar/dist/nebula/include/

LIBS = -lboost_system -lpthread 

objects = base64

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

