import base64
import time
import json
from wsgiref import headers
import requests
import asyncio
import hashlib
import hmac

# def createSignature(ApiSecret):
#     payload = {"request": "/v1/order/events","nonce": time.time(), "symbol":"btcusd", "amount":"1", "price":"9500", "side":"buy", "type":"exchange-limit", "options":["maker-or-cancel"]}
#     encoded_payload = json.dumps(payload).encode()
#     b64 = base64.b64encode(encoded_payload)
#     signature = hmac.new(ApiSecret, b64, hashlib.sha384).hexdigest()
#     infoList = [signature, b64.decode()]
#     return infoList


# async def geminiConnect(header):
#     async with websockets.connect(
#         "wss://api.sandbox.gemini.com/v1/order/new",
#         extra_headers=header) as websocket:
#             while websocket.open:
#                 response = await websocket.recv()
#                 print(json.loads(response))

# if __name__ == "__main__":

#     gemini_api_key = "master-6ABfTc6RZVzYP81dqNMa"
#     gemini_api_secret = "2PdHgBP3444p8vAykCv7f4CDpyNa".encode()
#     data = createSignature(gemini_api_secret);
#     header={
#         'X-GEMINI-PAYLOAD': data[1],
#         'X-GEMINI-APIKEY': gemini_api_key,
#         'X-GEMINI-SIGNATURE': data[0]
#     }
#     asyncio.run(geminiConnect(header))


def getHeader(apiSecret, apiKey, base64Payload):
    signature = hmac.new(apiSecret, base64Payload, hashlib.sha384).hexdigest()
    request_headers = {
        'Content-Type': "application/json",
        'Content-Length': "0",
        'X-GEMINI-APIKEY': apiKey,
        'X-GEMINI-PAYLOAD': base64Payload,
        'X-GEMINI-SIGNATURE': signature,
        'Cache-Control': "no-cache"
    }
    return request_headers


def newOrder(symbol, amount, price, side, type, options)->str:
    payload_nonce = time.time()
    # payload = {"request": "/v1/order/new", "nonce": payload_nonce, "symbol":"btcusd", "amount":"1", "price":"9454.75", "side":"buy", "type":"exchange limit", "options":["maker-or-cancel"]}
    payload = {"request": "/v1/order/new", "nonce": payload_nonce, "symbol":symbol, "amount":amount, "price":price, "side":side, "type":type, "options":options}
    encodedPayload = json.dumps(payload).encode()
    b64 = base64.b64encode(encodedPayload)
    return b64
    # signature = hmac.new(apiSecret, b64, hashlib.sha384).hexdigest()
    # request_headers = {
    #     'Content-Type': "application/json",
    #     'Content-Length': "0",
    #     'X-GEMINI-APIKEY': apiKey,
    #     'X-GEMINI-PAYLOAD': b64,
    #     'X-GEMINI-SIGNATURE': signature,
    #     'Cache-Control': "no-cache"
    # }
    # return request_headers

def orderCancel(orderId:int):
    payload = {"nonce": time.time(),"order_id": int(orderId),"request": "/v1/order/cancel"}
    encodedPayload = json.dumps(payload).encode()
    b64 = base64.b64encode(encodedPayload)
    return b64


if __name__=="__main__":
    url = "https://api.sandbox.gemini.com/"
    apiKey = "account-8BDIOhP2VnBAzQHAn6DT"
    apiSecret = "mXob3f85YcHu2KEQDxXeVSMtabL".encode()

    payload = newOrder("btcusd", "1", "9459.15", "buy", "exchange limit", ["maker-or-cancel"])
    header = getHeader(apiSecret, apiKey, payload)
    endpoint = "v1/order/new"
    response = requests.post(url+endpoint, headers=header)
    data = response.json()
    print(data)

    orderId = int(data["order_id"])
    payload = orderCancel(2141908063)
    header = getHeader(apiSecret, apiKey, payload)
    time.sleep(3)
    endpoint = "/v1/order/cancel"
    response = requests.post(url+endpoint, headers=header)
    print(response.json())

