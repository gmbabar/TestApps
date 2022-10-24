#!/usr/bin/python3
import json, hmac, hashlib, time, requests, base64
from requests.auth import AuthBase

#sandbox api keys and url
API_KEY='47dfc129c1135efb2348d443297aaa0c'
API_PASS='bdtvyfvpyu9'
API_SECRET='igIjLcJHQaBCMaYpGoexS2/HQ5jdKjXX7i4qsSAJkdcjb+deOkNHf0LfD9VDg9lGM+jfK5B6zSmyEASB0PYz1A=='
api_url='https://api-public.sandbox.exchange.coinbase.com/'

# Create custom authentication for Exchange
class CoinbaseExchangeAuth(AuthBase):
    def __init__(self, api_key, secret_key, passphrase):
        self.api_key = api_key
        self.secret_key = secret_key
        self.passphrase = passphrase

    def __call__(self, request):
        timestamp = str(int(time.time()))
        print('__call__(tms): ', str(timestamp))
        print('__call__(mtd): ', str(request.method))
        print('__call__(url): ', str(request.path_url))
        print('__call__(bdy): ', str(request.body))
        message = timestamp + request.method + request.path_url + (request.body or b'').decode()
        hmac_key = base64.b64decode(self.secret_key)
        signature = hmac.new(hmac_key, message.encode(), hashlib.sha256)
        signature_b64 = base64.b64encode(signature.digest()).decode()
        print('__call__(msg): ', str(message))
        print('__call__(sgn): ', str(signature.digest()))
        print('__call__(s64): ', str(signature_b64))
        print('--------------------------------------------')

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
    availableSymbols = []

    response = requests.get(api_url + 'products', auth=auth)
    contentType = response.headers['content-type']
    contentType = contentType[contentType.find("/")+1:contentType.find(";")].upper()

    if contentType == 'JSON' and response.status_code == 200:
        for symbols in response.json():
            availableSymbols.append(symbols['id'])
    
    print (f"Available Symbols:")
    for i in range(len(availableSymbols)):
        print(f"{i+1}: {availableSymbols[i]}")
    
