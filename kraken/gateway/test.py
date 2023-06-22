#!/usr/bin/python3
# import urllib.parse
# import hashlib
# import hmac
# import base64
# import requests
# import time

# def get_kraken_signature(urlpath, data, secret):

#     # postdata = urllib.parse.urlencode(data)
#     # print(postdata)
#     encoded = (str(data['nonce'])).encode()
#     message = urlpath.encode() + hashlib.sha256(encoded).digest()

#     mac = hmac.new(base64.b64decode(secret), message, hashlib.sha512)
#     sigdigest = base64.b64encode(mac.digest())
#     return sigdigest.decode()


# if __name__ == "__main__":


#     api_sec = "Kfx1vpSiT9VzmHzvJZAz1H6NZwET5HLQdaeIjJR3Dfe2zpBjGhMee0dhYFeuXwu2ZtKyC48L0FY05G8a8k9omw=="

#     data = {
#         "nonce": str(int(time.time()))
        
#     }

#     API_URL = "https://api.kraken.com"
#     API_KEY = "Liut6ciYIYn4QRGT6rT9xomWSNU2d25z4LSMwatGbyoB4eBmPbGNQ/Iu"
#     TARGET = "/0/private/Balance"
#     signature = get_kraken_signature(TARGET, data, api_sec)
#     HEADERS = {
#         'API-Key':API_KEY,
#         'API-Sign':signature,
#     }
#     result = requests.post(f"{API_URL}{TARGET}", headers=HEADERS)
#     print(result.status_code)
#     print(result.content)
#     # print(int(time.time()))



import hashlib
import hmac
import base64
import requests
import time
import json

def get_kraken_signature(urlpath, data, secret):
    postdata = json.dumps(data)
    # print((str(data['nonce']) + postdata))
    encoded = (str(data['nonce']) + postdata).encode()
    # print(str(data['nonce']))
    message = urlpath.encode() + hashlib.sha256(encoded).digest()
    # print(hashlib.sha256(encoded).digest())
    mac = hmac.new(base64.b64decode(secret), message, hashlib.sha512)
    # print(base64.b64decode(secret))
    sigdigest = base64.b64encode(mac.digest())
    # print(sigdigest.decode())
    return sigdigest.decode()

if __name__ == "__main__":
    api_sec = "Kfx1vpSiT9VzmHzvJZAz1H6NZwET5HLQdaeIjJR3Dfe2zpBjGhMee0dhYFeuXwu2ZtKyC48L0FY05G8a8k9omw=="

    data = {
        "nonce": int(time.time() * 1000)
    }
    # FOR BALANCES
    TARGET = "/0/private/Balance"
    #FOR OPEN ORDERS
    # TARGET = "/0/private/OpenOrders"
    signature = get_kraken_signature(TARGET, data, api_sec)
    API_KEY = "Liut6ciYIYn4QRGT6rT9xomWSNU2d25z4LSMwatGbyoB4eBmPbGNQ/Iu"
    print(int(time.time() * 1000))

    API_URL = "https://api.kraken.com"
    HEADERS = {
        'API-Key': API_KEY,
        'API-Sign': signature,
        'Content-Type': 'application/json'  # Set the content type to JSON
    }
    # print("===============================================")
    result = requests.post(f"{API_URL}{TARGET}", headers=HEADERS, data=json.dumps(data))
    print(result.status_code)
    print(result.json())