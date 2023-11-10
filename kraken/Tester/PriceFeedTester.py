#!/usr/bin/python3

import asyncio
import websockets
import json

msgs = {}
EXCHANGE_MULTIPLIER = 100000000

def compareMsg():
    exchMsg = msgs["exchange_msg"]
    internalMsg = msgs["internal_msg"]

    exchAsks = exchMsg["as"]
    exchBids = exchMsg["bs"]

    internalAsksPx = []
    internalBidsPx = []
    exchAsksPx = []
    exchBidsPx = []

    for idx in range(0, len(internalMsg)):
        if internalMsg[idx]["side"] == 1:
            internalBidsPx.append((internalMsg[idx]["price"]))
        else:
            internalAsksPx.append((internalMsg[idx]["price"]))

    for idx in range(0, len(exchAsks)):
        exchAsksPx.append(float(exchAsks[idx][0])*EXCHANGE_MULTIPLIER)
    for idx in range(0, len(exchBids)):
        exchBidsPx.append(float(exchBids[idx][0])*EXCHANGE_MULTIPLIER)

    priceMatched = True
    for idx in range(0, 10):
        if abs(internalAsksPx[idx] -  exchAsksPx[idx]) > 0.1:
            print(f"Ask({idx}) Exchange: {exchAsksPx[idx]}, Internal: {internalAsksPx[idx]} ---- Ask Mismatch - {idx}")
            priceMatched = False
        else:
            print(f"Ask({idx}) Exchange: {exchAsksPx[idx]}, Internal: {internalAsksPx[idx]}")

        if abs(internalBidsPx[idx] - exchBidsPx[idx]) > 0.1:
            print(f"Bid({idx}) Exchange: {exchBidsPx[idx]}, Internal: {internalBidsPx[idx]} ---- Bid Mismatch - {idx}")
            priceMatched = False
        else:
            print(f"Bid({idx}) Exchange: {exchBidsPx[idx]}, Internal: {internalBidsPx[idx]}")
        print("<====================================================>")
    if not priceMatched:
        print("Snapshot Price Levels Not Equal")
        return False

async def call_exchange_api(msg):
   async with websockets.connect('wss://ws.kraken.com/') as websocket:
       await websocket.send(json.dumps(msg))
       while websocket.open:
           response = await websocket.recv()
           response = json.loads(response)
        #    print(response)
           if type(response) is list and "as" in response[1]:
               msgs["exchange_msg"] = response[1]
               break

async def call_internal_api(msg):
   async with websockets.connect('ws://localhost:9015') as websocket:
       await websocket.send(json.dumps(msg))
       while websocket.open:
            response = await websocket.recv()
            response = json.loads(response)
            # print(response)
            # if response["type"] == "snapshot":
            # await process_message(response)
            msgs["internal_msg"] = response["levels"]
            break

async def main():
    exch_msg = {
        "event": "subscribe",
        "pair": ["TUSD/USD"],
        "subscription": {
            "name":"book"
        },
    }

    internal_msg = {
        "type":"subscribe",
        "exchange":"KRKN",
        "symbol":"TUSD/USD",
        "level":"S",
        "update_frequency":1,
        "time_window_secs":1,
        "max_levels":0,
        "one_shot":True}


    tasks = [
        call_exchange_api(exch_msg),
        call_internal_api(internal_msg)
    ]

    await asyncio.gather(*tasks)

if __name__ == "__main__":

    # internal_msg = {}
    asyncio.run(main())
    compareMsg()

