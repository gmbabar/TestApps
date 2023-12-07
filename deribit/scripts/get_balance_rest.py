#!/usr/bin/python3

import requests
import json
import hashlib
import hmac
import time

# Replace these with your Deribit API key and secret
API_KEY = '6d3m-Vq7'
API_SECRET = 'NuouYQA4KMfZrVdCuC9nKmNJ_HJsrY7r-dDMZZn9lPg'

# Deribit API URLs
BASE_URL = 'https://test.deribit.com/api/v2'  # Use 'https://www.deribit.com/api/v2' for the live platform

# Function to make a signed API request
def deribit_api_request(endpoint, params=None):
    if params is None:
        params = {}

    # Add authentication parameters
    params['jsonrpc'] = '2.0'
    params['id'] = 1
    params['method'] = endpoint
    params['params'] = {}

    # Get the current timestamp
    params['params']['timestamp'] = int(time.time() * 1000)

    # Create the request signature
    signature_payload = '/api/v2/' + endpoint + str(params['params']['timestamp']) + json.dumps(params['params'])
    signature = hmac.new(API_SECRET.encode('utf-8'), signature_payload.encode('utf-8'), hashlib.sha256).hexdigest()
    params['params']['sig'] = signature

    # Make the request
    response = requests.post(BASE_URL, json=params, headers={'Content-Type': 'application/json', 'Authorization': 'Bearer ' + API_KEY})

    # Parse the response
    return response.json()

# Function to get account balance
def get_account_balance():
    endpoint = 'private/get_account_summary'
    params = {'currency': 'BTC'}

    response = deribit_api_request(endpoint, params)
    if 'result' in response:
        balance = response['result']['equity']
        print(f'Account Balance: {balance} BTC')
    else:
        print('Error fetching account balance:', response)

# Example usage
if __name__ == '__main__':
    get_account_balance()

