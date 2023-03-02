CC = g++

# Mac OS only: /usr/local/opt/openssl/include
FLAGS = -g -std=c++11 -I./ -I/usr/local/opt/openssl/include -I../../json/rapidjson/include -I../../include
LIBS = -lboost_system -lpthread -lcrypto -lssl

objects = fix_api

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)


clean:
	rm -rf *.dSYM
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|common" | xargs`
