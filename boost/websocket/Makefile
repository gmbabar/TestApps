
CC = g++

# Mac OS only: /usr/local/opt/openssl/include
FLAGS = -g -std=c++11 -I./ -I/usr/local/opt/openssl/include
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = server server_ssl

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

# Inactive.
ServerSimulator: ServerSimulator.cpp
	g++ $(FLAGS) -o $@  $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|common" | xargs`

