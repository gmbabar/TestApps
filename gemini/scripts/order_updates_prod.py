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

# PROD
gemini_api_key = "account-lcDNZUZCQia4UgBOJFgD"
gemini_api_secret = "3Vij4y712WfhJcf3SNZHyJqaZZgD".encode()

payload = {"request": "/v1/order/events", "nonce": time.time()}
# payload = {"request": "/v1/order/events", "nonce": 1698209224}
encoded_payload = json.dumps(payload).encode()
b64 = base64.b64encode(encoded_payload)
signature = hmac.new(gemini_api_secret, b64, hashlib.sha384).hexdigest()

print('Payload: ', payload)
print('Payload: ', encoded_payload)

websocket.enableTrace(True)
ws = websocket.WebSocketApp("wss://api.gemini.com/v1/order/events",
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

