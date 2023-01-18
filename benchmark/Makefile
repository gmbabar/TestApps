
CC = g++

FLAGS = -g -I./ -I./include -march=native
# FLAGS = -g -I./
LIBS = -lpthread -lboost_system -lcrypto -lssl
objects = server client parser

all: $(objects)
$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|common" | xargs`

