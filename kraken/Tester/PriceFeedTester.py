#!/usr/bin/python3
import asyncio
import websockets
import json

msgs = {}

# async def process_message(msg):
#     askData = msg["as"]
#     print(f"Size Of ASK's {len(askData)}")
#     print()
#     print()
#     for idx in range(0, len(askData)):
#         print(askData[idx])
#     bidData = msg["bs"]
#     print(f"Size Of BID's {len(bidData)}")
#     print()
#     print()
#     for idx in range(0, len(bidData)):
#         print(bidData[idx])

# # {"price":99000000.000000,"qty":2020202000.000000,"qlen":1,"side":1}
# async def process_internal_message(msg):
#     print(msg)
#     data = msg["levels"]
#     print(f"Size Of Data {len(data)}")
#     print()
#     print()
#     bids = 0
#     asks = 0
#     for idx in range(0, len(data)):
#         print(data[idx])

#         if data[idx]["side"] == 1:
#             asks+=1
#         else:
#             bids+=1

#     # bidData = msg["bs"]
#     print()
#     print()
#     print(f"Size Of ASK's {asks}")
#     print(f"Size Of BID's {bids}")
#     #     print(bidData[idx])

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
            internalAsksPx.append(internalMsg[idx]["price"])
        else:
            internalBidsPx.append(internalMsg[idx]["price"])

    for idx in range(0, len(exchAsks)):
        exchAsksPx.append(float(exchAsks[idx][0]))
    for idx in range(0, len(exchBids)):
        exchBidsPx.append(float(exchBids[idx][0]))

    for idx in range(0, 10):
        if internalAsksPx[idx] ==  exchAsksPx[idx] and internalBidsPx[idx] == exchBidsPx[idx]:
            return True
        else:
            print("Snapshot Price Level Not Equal")
            return False

async def call_exchange_api(msg):
   async with websockets.connect('wss://ws.kraken.com/') as websocket:
       await websocket.send(json.dumps(msg))
       while websocket.open:
           response = await websocket.recv()
           response = json.loads(response)
           print(response)
           if type(response) is list and "as" in response[1]:
               msgs["exchange_msg"] = response[1]
               break



# {"type":"snapshot","globalSeq":14,"inside":{"bid":99000000.000000,"bidqty":2020202000.000000,"offer":100350000.000000,"offerqty":34771885000.000000,"exchange":"KRKN"},"isIntra":false,"key":[0,0],"live":1,"localSeq":0,"microsSinceEpoch":1692194649766835,"symbol":"TUSD/USD","levels":[{"price":99000000.000000,"qty":2020202000.000000,"qlen":1,"side":1},{"price":98340000.000000,"qty":699159605000.000000,"qlen":1,"side":1},{"price":98330000.000000,"qty":47784155000.000008,"qlen":1,"side":1},{"price":97880000.000000,"qty":703194178000.000000,"qlen":1,"side":1},{"price":97770000.000000,"qty":701552090000.000000,"qlen":1,"side":1},{"price":97540000.000000,"qty":634104346000.000122,"qlen":1,"side":1},{"price":97530000.000000,"qty":717018020000.000122,"qlen":1,"side":1},{"price":97370000.000000,"qty":692439107000.000000,"qlen":1,"side":1},{"price":97260000.000000,"qty":732665176000.000122,"qlen":1,"side":1},{"price":97110000.000000,"qty":699818347000.000122,"qlen":1,"side":1},{"price":97100000.000000,"qty":703906077000.000000,"qlen":1,"side":1},{"price":96930000.000000,"qty":716660354000.000000,"qlen":1,"side":1},{"price":96920000.000000,"qty":687932972000.000122,"qlen":1,"side":1},{"price":96740000.000000,"qty":500000000.000000,"qlen":1,"side":1},{"price":95670000.000000,"qty":14326275000.000002,"qlen":1,"side":1},{"price":92500000.000000,"qty":154354657000.000031,"qlen":1,"side":1},{"price":90240000.000000,"qty":51028090000.000000,"qlen":1,"side":1},{"price":85470000.000000,"qty":634479864000.000000,"qlen":1,"side":1},{"price":85460000.000000,"qty":634469166000.000000,"qlen":1,"side":1},{"price":85450000.000000,"qty":634389631000.000000,"qlen":1,"side":1},{"price":85440000.000000,"qty":634416842000.000000,"qlen":1,"side":1},{"price":85420000.000000,"qty":634453523000.000000,"qlen":1,"side":1},{"price":85410000.000000,"qty":634463704000.000000,"qlen":1,"side":1},{"price":85400000.000000,"qty":634432564000.000000,"qlen":1,"side":1},{"price":85390000.000000,"qty":634597868000.000000,"qlen":1,"side":1},{"price":100350000.000000,"qty":34771885000.000000,"qlen":1,"side":-1},{"price":100360000.000000,"qty":13469640000.000002,"qlen":1,"side":-1},{"price":100440000.000000,"qty":4978534584000.000000,"qlen":1,"side":-1},{"price":101000000.000000,"qty":4420000.000000,"qlen":1,"side":-1},{"price":102050000.000000,"qty":994333000.000000,"qlen":1,"side":-1},{"price":150000000.000000,"qty":500000000.000000,"qlen":1,"side":-1},{"price":750000000.000000,"qty":500000000.000000,"qlen":1,"side":-1},{"price":22200000000.000000,"qty":500000000.000000,"qlen":1,"side":-1},{"price":22300000000.000000,"qty":950000000.000000,"qlen":1,"side":-1},{"price":742500000000.000000,"qty":500000000.000000,"qlen":1,"side":-1},{"price":742700000000.000000,"qty":2500000000.000000,"qlen":1,"side":-1}]}
async def call_internal_api(msg):
   async with websockets.connect('wss://ws.kraken.com/') as websocket:
       await websocket.send(json.dumps(msg))
       while websocket.open:
            response = await websocket.recv()
            response = json.loads(response)
            print(response)
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

    internal_msg = {}

    tasks = [
        call_exchange_api(exch_msg),
        call_internal_api(internal_msg)
    ]

    await asyncio.gather(*tasks)

if __name__ == "__main__":

    # internal_msg = {}
    asyncio.run(main())
    compareMsg()

