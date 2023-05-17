#!/usr/bin/python3
import requests
import hashlib
import hmac
import time
from base64 import b64decode, b64encode
import random

client_key_id = '27d74ff768ee430626df8341763a8de1'
client_secret = 'Fm1cu6WZpQDppCFDoROTVA=='
nonce = str(random.randint(1000, 9999))
timestamp = str(int(time.time()))
signature = b64encode(hmac.new(b64decode(client_secret), msg = bytes(client_key_id + nonce + timestamp, 'utf-8'), digestmod = hashlib.sha256).digest()).decode('utf-8')

API_URL = 'https://account-api.digital-uat.lmax.com'
# client_key_id = '2a6aed0de9f875c2407bc693034b477f'
# nonce = 'nonce'
# timestamp = '2021-10-21T01:43:28.786Z'

r = requests.post(API_URL + '/v1/authenticate', json = {'client_key_id': client_key_id, 'timestamp': timestamp, 'nonce': nonce, 'signature': signature})
if r.status_code != 200:
    raise Exception('Failed to authenticate: ' + r.text)

auth_token = r.json()['token']
print(auth_token)