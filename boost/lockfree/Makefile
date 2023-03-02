
CC = g++

FLAGS = -g -std=c++11 -I./ 

LIBS = -lboost_system -lpthread 

objects = first second third spsc_cls_obj spsc_cls_ptr spsc_cls_test

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f $(objects)

