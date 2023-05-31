
CC = g++

FLAGS = -std=c++17 -g -I./  -I../../include
LIBS = -lboost_system -lssl -lcrypto

# objects = gemini_rest_send_order gemini_rest_symbols test_cli gemini_rest_cancel gemini_formatter gemini_rest_order_status
# objects = gemini_websocket
objects =  parser


all: $(objects)

$(objects): %: %.cpp
	$(CC) $(FLAGS) -o $@ $< $(LIBS)
# {"method": "subscribe","ch": "trades","params": {"symbols": ["ETHBTC", "BTCUSDT"],"limit": 1},"id": 123}
clean:
	rm -rf *.dSYM
	rm -f $(objects)

