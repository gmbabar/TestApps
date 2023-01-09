
CC = g++

FLAGS = -g -I./ -march=native
# FLAGS = -g -I./ -march=noavx
# FLAGS = -g -I./
LIBS = -lpthread -lboost_system -lcrypto -lssl
objects = server client tcp_client_ssl tcp_server_ssl

all: $(objects)
$(objects): %: %.cpp
	$(CC) $(FLAGS) -o ./bin/$@ $< $(LIBS)

clean:
	rm -rf *.dSYM
	rm -f `ls | egrep -v "Makefile|\.cpp|\.hpp|\.h|common" | xargs`

