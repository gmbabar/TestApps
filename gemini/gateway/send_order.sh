#!/usr/bin/bash

echo g++ gemini_rest_send_order.cpp -o gemini_rest_send_order -g -std=c++17 -I./  -I../../include -lboost_system -lssl -lcrypto -lpthread
g++ gemini_rest_send_order.cpp -o gemini_rest_send_order -g -std=c++17 -I./  -I../../include -lboost_system -lssl -lcrypto -lpthread
echo ./gemini_rest_send_order
./gemini_rest_send_order
