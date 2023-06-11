#!/usr/bin/python3
import requests
import hashlib
import hmac
import time
from base64 import b64decode, b64encode
import datetime

'''
LmaxG-UAT:
web-endpoint: https://account-api.london-uat.lmax.com
client_key: 0d554c092803e2c1dca579a5ca3d09fa
client_sec: ODHvLOuy81+lOPul56O8Ng==
- - - - - -
LmaxD-UAT:
web-endpoint: https://account-api.digital-uat.lmax.com
client_key: 27d74ff768ee430626df8341763a8de1
client_sec: Fm1cu6WZpQDppCFDoROTVA==
'''

import textwrap
def print_roundtrip(response, *args, **kwargs):
    format_headers = lambda d: '\n'.join(f'{k}: {v}' for k, v in d.items())
    print(textwrap.dedent('''
        ---------------- request ----------------
        {req.method} {req.url}
        {reqhdrs}

        {req.body}
        ---------------- response ----------------
        {res.status_code} {res.reason} {res.url}
        {reshdrs}

        {res.text}
    ''').format(
        req=response.request,
        res=response,
        reqhdrs=format_headers(response.request.headers),
        reshdrs=format_headers(response.headers),
    ))

client_key_id = '27d74ff768ee430626df8341763a8de1'
client_secret = 'Fm1cu6WZpQDppCFDoROTVA=='
nonce = str(time.time())
timestamp = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S.000Z")

signature = b64encode(hmac.new(b64decode(client_secret), msg = bytes(client_key_id + nonce + timestamp, 'utf-8'), digestmod = hashlib.sha256).digest()).decode('utf-8')

API_URL = 'https://account-api.digital-uat.lmax.com'
JSON = {'client_key_id': client_key_id, 'timestamp': timestamp, 'nonce': nonce, 'signature': signature}

resp = requests.post(API_URL + '/v1/authenticate', json = JSON, hooks={'response': print_roundtrip})
if resp.status_code != 200:
    raise Exception('Failed to authenticate: ' + resp.text)

# auth_token = req.json()['token']
print(JSON)
token = resp.json()['token']
header = {'Authorization':f"Bearer {token}"}
r=requests.get(API_URL + '/v1/account/wallets', headers=header, hooks={'response': print_roundtrip})
# print(token)
# print(header)
# print(timestamp)
print(r.json())
