from asyncio.windows_events import NULL
import ssl
import websockets
import asyncio
import json
import base64
import hmac
import hashlib
import time

def createSignature(ApiSecret):
    payload = {"request": "/v1/order/events","nonce": time.time(), "symbol":"btcusd", "amount":"1", "price":"9500", "side":"buy", "type":"exchange-limit", "options":["maker-or-cancel"]}
    encoded_payload = json.dumps(payload).encode()
    b64 = base64.b64encode(encoded_payload)
    signature = hmac.new(ApiSecret, b64, hashlib.sha384).hexdigest()
    infoList = [signature, b64.decode()]
    return infoList


async def geminiConnect(header):
    async with websockets.connect(
        "wss://api.sandbox.gemini.com/v1/order/new",
        extra_headers=header) as websocket:
            while websocket.open:
                response = await websocket.recv()
                print(json.loads(response))


                

if __name__ == "__main__":

    gemini_api_key = "master-6ABfTc6RZVzYP81dqNMa"
    gemini_api_secret = "2PdHgBP3444p8vAykCv7f4CDpyNa".encode()
    data = createSignature(gemini_api_secret);
    header={
        'X-GEMINI-PAYLOAD': data[1],
        'X-GEMINI-APIKEY': gemini_api_key,
        'X-GEMINI-SIGNATURE': data[0]
    }
    asyncio.run(geminiConnect(header))