#!/bin/bash

./gdax_pricefeed ws-feed.exchange.coinbase.com 443 "{\"type\":\"subscribe\",\"product_ids\":[\"ETH-USD\",\"ETH-EUR\"],\"channels\":[\"level2\",\"heartbeat\",{\"name\":\"ticker\",\"product_ids\":[\"ETH-BTC\",\"ETH-USD\"]}]}"

