#!/usr/bin/python

import requests
import json
import base64
import hmac
import hashlib
import datetime, time

base_url = "https://api.gemini.com"
endpoint = "/v1/balances"
url = base_url + endpoint

# Production (personal) Keys
gemini_api_key = "account-lcDNZUZCQia4UgBOJFgD"
gemini_api_secret = "3Vij4y712WfhJcf3SNZHyJqaZZgD".encode()

# # Staging Keys
# gemini_api_key = "account-DOg0jQYwTa5cqWAcMjmY"
# gemini_api_secret = "xFf2j6QzYMRAt5MsMztGgMGi41".encode()

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

