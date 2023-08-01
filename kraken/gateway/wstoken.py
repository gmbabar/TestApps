#!/usr/bin/python3

import hashlib
import hmac
import base64
import requests
import time
import json

def get_kraken_signature(urlpath, data, secret):
    postdata = json.dumps(data)
    encoded = (str(data['nonce']) + postdata).encode()
    message = urlpath.encode() + hashlib.sha256(encoded).digest()
    mac = hmac.new(base64.b64decode(secret), message, hashlib.sha512)
    sigdigest = base64.b64encode(mac.digest())
    return sigdigest.decode()

if __name__ == "__main__":
    api_key = 'pZW/cAyUQfpPnubzo1v3Vj6KlDT7QklrmNCJiPDlRNxZLvRqjDuWawod'
    api_sec = 'jQSIs/VFwIME1BD74THcV5irnTG0/nzPNgBK6lfCebhqHMqONSlGARWqm1wLrOjYqFWW86/eV01jNv5cIQhM5g=='


    # data = {
    #     "nonce": int(time.time() * 1000)
    #     ,"pair":"XBTUSD",
    # }
    data = {
        "nonce": int(time.time() * 1000)
    }

    # New Order
    TARGET = "/0/private/GetWebSocketsToken"

    signature = get_kraken_signature(TARGET, data, api_sec)

    API_URL = "https://api.kraken.com"
    HEADERS = {
        'API-Key': api_key,
        'API-Sign': signature,
        'Content-Type': 'application/json'  # Set the content type to JSON
    }

    print ('URL: ', API_URL+TARGET)
    print ('Data: ', data)
    print ('Headers: ', HEADERS)
    print("===============================================")
    result = requests.post(f"{API_URL}{TARGET}", headers=HEADERS, data=json.dumps(data))
    print(result.status_code)
    print(result.json())
