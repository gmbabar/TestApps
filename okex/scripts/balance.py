#!/usr/bin/python3
import asyncio
import websockets
import json
import base64
import hmac
import hashlib
import time
# To subscribe to this channel:

API_KEY = "415c3127-6b61-478f-bd21-94c946bd7a62"
API_SECRET = "91878CFAFE56860490C2F977FDCEBAF7".encode('utf-8')
PASS_PHRASE = "ForTestingOnTST.1"
TIMESTAMP = str(int(time.time()))



def generateSignature():
    payload = str(TIMESTAMP)+ "GET" + "/users/self/verify"
    mac = hmac.new(API_SECRET, payload.encode('utf-8'), hashlib.sha256)
    signature = base64.b64encode(mac.digest()).decode()
    return signature


msg = json.dumps(
    {
        "op": "login",
        "args": [
            {
                "apiKey": API_KEY,
                "passphrase": PASS_PHRASE,
                "timestamp": str(TIMESTAMP),
                "sign": generateSignature()
            }
        ]
    })

subscription = json.dumps(
    {
        "op": "subscribe",
        "args": [{
            "channel": "balance_and_position"
        }]
    })

subscription2 = json.dumps(
    {
        "op": "subscribe",
        "args": [
            {
                "channel": "orders",
                "instType": "ANY"
            }
        ]
    })

headers = {'x-simulated-trading': 1}

async def login():
    async with websockets.connect('wss://wspap.okx.com:8443/ws/v5/private?brokerId=9999', extra_headers=headers) as websocket:
        await websocket.send(msg)
        response = await websocket.recv()
        print(f"Response Received: {json.loads(response)}")
        await websocket.send(subscription)
        response = await websocket.recv()
        print(f"Response Received: {json.loads(response)}")
        await websocket.send(subscription2)
        while websocket.open:
            response = await websocket.recv()
            print(f"Response Received: {json.loads(response)}")


if __name__ == "__main__":
    asyncio.run(login())