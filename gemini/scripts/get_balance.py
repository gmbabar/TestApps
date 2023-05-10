#!/usr/bin/python

import requests
import json
import base64
import hmac
import hashlib
import datetime, time

base_url = "https://api.sandbox.gemini.com"
endpoint = "/v1/balances"
url = base_url + endpoint

gemini_api_key = "account-8BDIOhP2VnBAzQHAn6DT"
gemini_api_secret = "mXob3f85YcHu2KEQDxXeVSMtabL".encode()

t = datetime.datetime.now()
payload_nonce = time.time()

payload = {
   "request": endpoint,
    "nonce": payload_nonce
}

encoded_payload = json.dumps(payload).encode()
b64 = base64.b64encode(encoded_payload)
signature = hmac.new(gemini_api_secret, b64, hashlib.sha384).hexdigest()

request_headers = { 'Content-Type': "text/plain",
                    'Content-Length': "0",
                    'X-GEMINI-APIKEY': gemini_api_key,
                    'X-GEMINI-PAYLOAD': b64,
                    'X-GEMINI-SIGNATURE': signature,
                    'Cache-Control': "no-cache" }

print("url:", url)
print("payload:", payload)
print('headers:', request_headers)
response = requests.post(url,
                        data=None,
                        headers=request_headers)

balances = response.json()
print('RESPONSE:')
print(balances)

