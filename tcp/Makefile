
CC = g++

FLAGS = -g -I./ 
LIBS = -lpthread

objects = server client

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|common" | xargs`

