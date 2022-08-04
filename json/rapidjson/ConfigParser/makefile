CC = g++
FLAGS =  -g -Wall -lboost_system
LIBS = -lboost_system

objects = ParseConfig

all:$(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)


clean:
	rm -rf *.dSYM
	rm -f `find . -maxdepth 1 -type f | egrep -v "makefile|\.json|\.cpp|\.sh|\.hpp|\.h|include|git" | xargs`