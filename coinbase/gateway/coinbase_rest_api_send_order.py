import json, hmac, hashlib, time, requests, base64
from requests.auth import AuthBase

#sandbox api keys and url
API_KEY='47dfc129c1135efb2348d443297aaa0c'
API_PASS='bdtvyfvpyu9'
API_SECRET='igIjLcJHQaBCMaYpGoexS2/HQ5jdKjXX7i4qsSAJkdcjb+deOkNHf0LfD9VDg9lGM+jfK5B6zSmyEASB0PYz1A=='
api_url='https://public.sandbox.pro.coinbase.com/'

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
        timestamp = str(time.time())
        message = timestamp + request.method + request.path_url + (request.body or b'').decode()
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

api_url='https://api-public.sandbox.exchange.coinbase.com/'
auth = CoinbaseExchangeAuth(API_KEY, API_SECRET, API_PASS)

order = {
    'size': 10,
    'price': 9950,
    'side': 'buy',
    'product_id': 'BTC-USD',
}
r = requests.post(api_url + 'orders', json=order, auth=auth)
data = r.json()
# print(f"Created A {data['side']} Order Of {data['price']}@{data['size']}, ID = {data['id']}")
print (f'data: {data}')
