
CC = g++

#FLAGS = -I./ -march=native -mno-avx -O3
FLAGS = -I./ -march=native -O3
#FLAGS = -I./ -O3
# FLAGS = -g -I./
LIBS = -lpthread -lboost_system -lcrypto -lssl
objects = echo_server echo_client server client parser tcp_client_ssl tcp_server_ssl

all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|common|\.sh" | xargs`

