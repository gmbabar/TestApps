#!/usr/bin/python3

import websockets
import asyncio
import json
import hmac
import hashlib
import time

# Replace these with your Deribit API key and secret
API_KEY = '6d3m-Vq7'
API_SECRET = 'NuouYQA4KMfZrVdCuC9nKmNJ_HJsrY7r-dDMZZn9lPg'

# Deribit WebSocket API URL
WS_URL = 'wss://test.deribit.com/ws/api/v2'  # Use 'wss://www.deribit.com/ws/api/v2' for the live platform

async def authenticate_and_get_balance():
    async with websockets.connect(WS_URL) as ws:
        # Construct authentication message
        timestamp = int(time.time() * 1000)
        nonce = str(timestamp)
        data = ""
        # signature_payload = f'{timestamp}\n{nonce}\n/wapi/v2/private/get_account_summary'
        signature_payload = f'{timestamp}\n{nonce}\n{data}'
        signature = hmac.new(API_SECRET.encode('utf-8'), signature_payload.encode('utf-8'), hashlib.sha256).hexdigest()

        auth_message = {
            "jsonrpc": "2.0",
            "id": 1,
            "method": "public/auth",
            "params": {
                "grant_type": "client_signature",
                "client_id": API_KEY,
                "timestamp": timestamp,
                "signature": signature,
                "nonce": nonce,
                "data": data
            }
        }
        print(f'Authentication Request: {auth_message}')

        # Send authentication message
        await ws.send(json.dumps(auth_message))

        # Receive and print authentication response
        auth_response = await ws.recv()
        print(f'Authentication Response: {auth_response}')

        # Construct message to get account summary (balances)
        get_balance_message = {
            "jsonrpc": "2.0",
            "id": 2,
            "method": "private/get_account_summary",
            "params": {
                "currency": "BTC"
            }
        }

        # Send message to get account summary
        await ws.send(json.dumps(get_balance_message))

        # Receive and print account summary response
        balance_response = await ws.recv()
        print(f'Account Summary Response: {balance_response}')

# Run the event loop
asyncio.get_event_loop().run_until_complete(authenticate_and_get_balance())
