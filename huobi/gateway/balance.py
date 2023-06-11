#!/usr/bin/python3
import asyncio
import websockets
import json
import hmac
import hashlib
from urllib import parse
from datetime import datetime
import base64

# Huobi WebSocket API URL
websocket_url = 'wss://api.huobi.pro/ws/v2'

# API Key and Secret
API_KEY = '7d393344-1qdmpe4rty-37def1b5-d10e3'
API_SECRET = '84aadfc7-a69c48d2-e5538ffc-e0f8c'

# Generate the authentication signature
def generate_signature():
    params = {
            'accessKey': API_KEY,
            'signatureMethod': 'HmacSHA256',
            'signatureVersion': '2.1',
            'timestamp': datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%S')
        }
    target = sorted(params.items(),
                        key=lambda x: x[0], reverse=False)  # 参数ASCII排序ta
    to_sign = '\n'.join(
            ['GET', 'api.huobi.pro', '/ws/v2', parse.urlencode(target)])

    params.update({
            'signature': base64.b64encode(hmac.new(API_SECRET.encode(),
                            to_sign.encode(), hashlib.sha256).digest()).decode(),
            'authType': 'api'
        })

    return params

# Define the authentication request
def authenticate():
    params = generate_signature()
    auth_payload = {
        'action': 'req',
        'ch': 'auth',
        'params':params
    }
    return json.dumps(auth_payload)

def subscribe(channel:str):
    msg = {
        "action": "sub",
        "ch": channel
    }
    return json.dumps(msg)

# Define the main connection and message handling logic
async def connect_to_huobi():
    async with websockets.connect(websocket_url) as ws:
        auth_request = authenticate()
        await ws.send(auth_request)
        message = await ws.recv()
        print(message)
        await ws.send(subscribe("accounts.update"))
        while ws.open:
            message = await ws.recv()
            print(message)

if __name__ == "__main__":
    asyncio.run(connect_to_huobi())
