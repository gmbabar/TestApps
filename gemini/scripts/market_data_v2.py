#!/usr/bin/python3

import ssl
import websocket
import _thread as thread

def on_message(ws, message):
    print(message)

def on_error(ws, error):
    print(error)

def on_close(ws):
    print("### closed ###")

def on_open(ws):
    def run(*args):
        ws.send(logon_msg)
    thread.start_new_thread(run, ())

if __name__ == "__main__":
    # Subscript types (name): l2, candles_1m, candles_5m, candles_15m, ...
    #logon_msg = '{"type": "subscribe","subscriptions":[{"name":"l2","symbols":["BTCUSD","ETHUSD","ETHBTC"]}]}'
    #logon_msg = '{"type": "subscribe","subscriptions":[{"name":"l2","symbols":["ETHUSD"]}]}'
    logon_msg = '{"type": "subscribe","subscriptions":[{"name":"l2","symbols":["ethusd"]}]}'
    #logon_msg = '{"type": "subscribe","subscriptions":[{"name":"candles_1m","symbols":["BTCUSD"]}]}'
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("wss://api.gemini.com/v2/marketdata",
                                on_message = on_message,
                                on_error = on_error,
                                on_close = on_close,
                                on_open = on_open)
    ws.on_open = on_open
    ws.run_forever(sslopt={"cert_reqs": ssl.CERT_NONE})


