#!/usr/bin/python3
import json, hmac, hashlib, time, requests, base64
from requests.auth import AuthBase

#sandbox api keys and url
API_KEY='47dfc129c1135efb2348d443297aaa0c'
API_PASS='bdtvyfvpyu9'
API_SECRET='igIjLcJHQaBCMaYpGoexS2/HQ5jdKjXX7i4qsSAJkdcjb+deOkNHf0LfD9VDg9lGM+jfK5B6zSmyEASB0PYz1A=='
# api_url='https://public.sandbox.pro.coinbase.com/'
api_url='https://api-public.sandbox.exchange.coinbase.com/'

class bcolors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    RED = '\033[1;31m'
    YELLOW = '\033[1;33m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'


def jsonPrint(obj):
    # create a formatted string of the Python JSON object
    text = json.dumps(obj, sort_keys=True, indent=4)
    print(text)

# Create custom authentication for Exchange
class CoinbaseExchangeAuth(AuthBase):
    def __init__(self, api_key, secret_key, passphrase):
        self.api_key = api_key
        self.secret_key = secret_key
        self.passphrase = passphrase

    def __call__(self, request):
        timestamp = str(int(time.time()))
        print('__call__', str(timestamp))
        print('__call__', str(request.method))
        print('__call__', str(request.path_url))
        print('__call__', str(request.body))
        message = timestamp + request.method + request.path_url + (request.body or b'').decode()
        print('__call__', str(message))
        hmac_key = base64.b64decode(self.secret_key)
        signature = hmac.new(hmac_key, message.encode(), hashlib.sha256)
        signature_b64 = base64.b64encode(signature.digest()).decode()

        request.headers.update({
            'CB-ACCESS-SIGN': signature_b64,
            'CB-ACCESS-TIMESTAMP': timestamp,
            'CB-ACCESS-KEY': self.api_key,
            'CB-ACCESS-PASSPHRASE': self.passphrase,
            'Content-Type': 'application/json'
        })
        return request

if __name__=="__main__":

    auth = CoinbaseExchangeAuth(API_KEY, API_SECRET, API_PASS)
    
    order = {
        'size': 0.001,
        'price': 9500,
        'side': 'buy',
        'product_id': 'BTC-EUR',
        'time_in_force': 'GTC'
    }
    response = requests.post(api_url + 'orders', json=order, auth=auth)
    data = response.json()
    if 'side' in data and 'price' in data and 'size' in data:
        print('response:', data)
        print(f"{bcolors.OKGREEN}Created A {data['side']} Order Of {data['price']}@{data['size']}, ID = {data['id']}, Symbol = {data['product_id']}{bcolors.ENDC}")
    else:
        print(f"{bcolors.FAIL}{data}{bcolors.ENDC}")
    
    # response = requests.get(api_url + 'fills', auth=auth)
    # data = response.json()
    # print(data)
