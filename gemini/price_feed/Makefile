CC = g++

# Mac OS only: /usr/local/opt/openssl/include
FLAGS = -g -std=c++11 -I./ -I/usr/local/opt/openssl/include -I../include
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = gemini_pf

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)


clean:
	rm -rf *.dSYM
	# rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|include" | xargs`
	rm -f `find . -maxdepth 1 -type f | egrep -v "Makefile|\.json|\.cpp|\.sh|\.hpp|\.h|include|git" | xargs`

