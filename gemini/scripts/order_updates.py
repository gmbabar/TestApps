#!/usr/bin/python3

import ssl
import websocket
import json
import base64
import hmac
import hashlib
import time

def on_message(ws, message):
    print(message)

def on_error(ws, error):
    print(error)

def on_close(ws):
    print("### closed ###")

def on_open(ws):
    print("Opened connection")

gemini_api_key = "account-8BDIOhP2VnBAzQHAn6DT"
gemini_api_secret = "mXob3f85YcHu2KEQDxXeVSMtabL".encode()

payload = {"request": "/v1/order/events","nonce": time.time()}
encoded_payload = json.dumps(payload).encode()
b64 = base64.b64encode(encoded_payload)
signature = hmac.new(gemini_api_secret, b64, hashlib.sha384).hexdigest()

print('Payload: ', payload)

websocket.enableTrace(True)
ws = websocket.WebSocketApp("wss://api.sandbox.gemini.com/v1/order/events",
                            on_message=on_message,
                            on_open=on_open,
                            on_error=on_error,
                            on_close=on_close,
                            header={
                                'X-GEMINI-PAYLOAD': b64.decode(),
                                'X-GEMINI-APIKEY': gemini_api_key,
                                'X-GEMINI-SIGNATURE': signature
                            })
ws.run_forever(sslopt={"cert_reqs": ssl.CERT_NONE})

