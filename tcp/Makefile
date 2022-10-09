
CC = g++

FLAGS = -g -I./ 
LIBS = -lpthread

objects = server client jcx_md_simulator 

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|common" | xargs`

