#!/usr/bin/python3

import sys
import json
import websocket
import time
import rel


def on_open(ws):
    ws.send(json.dumps({"type":"security_list_request"}))

def on_message(ws, message):
    j = json.loads(message)

    print(j)
    msgtype = j["type"]
    print(msgtype)
    if msgtype == "security_list":
        symbols = j["symbols"]
        for sym in symbols:
            exch, symbol = sym.split(":")
            ws.send(json.dumps({"type":"subscribe", "exchange":exch, "symbol":symbol, "level":"S", "max_levels":5, "one_shot": True}))

def on_error(ws, error):
    print(error)

def on_close(ws, close_status_code, close_msg):
    print("### closed ###")

if __name__ == "__main__":
    start_ts = time.time()
    sent_second_oneshot = False
    if len(sys.argv) < 2:
        print("Usage:")
        print("\t", sys.argv[0], " <url> ")
        print("\t", sys.argv[0], " ws://localhost:9007 ")
        sys.exit(0)
    url = sys.argv[1]

    while True:
        ws = websocket.WebSocketApp(url,
                                    on_open=on_open,
                                    on_message=on_message,
                                    on_error=on_error,
                                    on_close=on_close)
        ws.run_forever()
        rel.signal(2, rel.abort)
        rel.dispatch()


