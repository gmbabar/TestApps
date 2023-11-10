#!/usr/bin/python3

import requests
import json
import hashlib
import hmac
import time

# Replace with your own Binance API credentials
API_KEY = 'c5986072fb1edf72dc0dc230c7495153880440348dd3c98125aff04d5eb0f482'
SECRET_KEY = 'd34e95ac0feace97873e35857fbee7b61acb58f727f98b1b7d1dbd88a4a086d0'

# Binance API endpoints
# BASE_URL = 'https://fapi.binance.com'     # prod
BASE_URL = 'https://testnet.binancefuture.com'       # test
ACCOUNT_ENDPOINT = '/fapi/v2/account'

# Create a timestamp for the request
timestamp = int(time.time() * 1000)

# Create the query string
query_string = f'timestamp={timestamp}'

# Create the signature
signature = hmac.new(SECRET_KEY.encode('utf-8'), query_string.encode('utf-8'), hashlib.sha256).hexdigest()

# Set up the request headers
headers = {
    'X-MBX-APIKEY': API_KEY
}

# Make the API request
url = BASE_URL + ACCOUNT_ENDPOINT + '?' + query_string + '&signature=' + signature
response = requests.get(url, headers=headers)

# Parse the JSON response
data = response.json()

# Print the account balance
if 'assets' in data:
    for asset in data['assets']:
        # if float(asset['walletBalance']) > 0:
        print(f"Asset: {asset['asset']}, Balance: {asset['walletBalance']}, availableBalance: {asset['availableBalance']}")
else:
    print("Error fetching account balance:", data)

