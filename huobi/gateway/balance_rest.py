#!/usr/bin/python3

import hmac
import hashlib
from urllib import parse
from datetime import datetime
import base64
import requests

'''
- Gather message to do authentication signature:
    GET\n
    api.huobi.pro\n
    /v1/account/accounts\n
    AccessKeyId={api_key}&SignatureMethod=HmacSHA256&SignatureVersion=2&Timestamp={timestamp}

- Signature:


- Final:
    https://api.huobi.pro/v1/account/accounts?
        AccessKeyId=e2xxxxxx-99xxxxxx-84xxxxxx-7xxxx            # api_key_id
        &SignatureMethod=HmacSHA256
        &SignatureVersion=2
        &Timestamp=2017-05-11T15%3A19%3A30
        &Signature=4F65x5A2bLyMWVQj3Aqp%2BB4w%2BivaA7n5Oi2SuYtCJ9o%3D


PROD:
    /v1/account/accounts?
        AccessKeyId=7d393344-1qdmpe4rty-37def1b5-d10e3
        &SignatureMethod=HmacSHA256
        &SignatureVersion=2
        &Timestamp=2023-07-04T20%3A07%3A06
        &Signature=SuqFdT%2FA6ThdjAGGuTEvG9doiOcoqve1SYdDvIWVv0w%3D
MINE:
    /v1/account/accounts?
        AccessKeyId=7d393344-1qdmpe4rty-37def1b5-d10e3
        &SignatureMethod=HmacSHA256
        &SignatureVersion=2
        &Timestamp=2023-07-04T20%3A07%3A06
        &Signature=Oc0nORKejxXRzIa2NGjzJKUFobf1YxHklBYj%2BtGpGNQ%3D
'''


# Huobi WebSocket API URL
rest_url = 'https://api.huobi.pro/v1/account/accounts?'

# API Key and Secret
API_KEY = '7d393344-1qdmpe4rty-37def1b5-d10e3'
API_SECRET = '84aadfc7-a69c48d2-e5538ffc-e0f8c'

# API_KEY = '34d811df-289a1500-dqnh6tvdf3-3a175'
# API_SECRET = '54635bfc-0a9366a6-fea5b3d6-727b6'

# Generate the authentication signature
def generate_request():
    params = {
            'AccessKeyId': API_KEY,
            'SignatureMethod': 'HmacSHA256',
            'SignatureVersion': '2',
            'Timestamp': datetime.utcnow().strftime('%Y-%m-%dT%H:%M:%S')
        }
    to_sign = '\n'.join(
            ['GET', 'api.huobi.pro', '/v1/account/accounts', parse.urlencode(params)])

    params.update({
            'Signature': base64.b64encode(hmac.new(API_SECRET.encode(),
                            to_sign.encode(), hashlib.sha256).digest()).decode()
        })

    return rest_url + parse.urlencode(params)

if __name__ == "__main__":
    print (generate_request())

    response = requests.get(generate_request())
    data = response.json()
    print(data)

