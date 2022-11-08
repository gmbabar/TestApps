#!/usr/bin/python3
import json
import requests

# Testing URL = api.sandbox.gemini.com/v1
api_url='https://api.gemini.com/v1/symbols'
symbolDetails = "https://api.gemini.com/v1/symbols/details/"

def printInstruments(instruments:list):
    print("[")
    for instrument in instruments:
        print(f"\t{instrument}")
    print("]")

def gemini_instrument_loader(params):
    instruments = []
    response = requests.get(api_url)
    symbols = response.json()
    for symbol in symbols:
        response =  requests.get(f"https://api.gemini.com/v1/symbols/details/{symbol}")
        data = response.json()
        if (data['status'] == "open" or data['status'] == 'limit_only'):
            exchSym = data['symbol']
            baseCcy = data['base_currency']
            quoteCcy = data['quote_currency']
            tickSize = data['tick_size']
            final_data = {
                "Exchange-Symbol":exchSym,
                "QPT-Symbol":baseCcy+quoteCcy,
                "Quote-Currency":quoteCcy,
                "Root-Symbol":baseCcy,
                "Tick-Size":tickSize
            }
            print("Appending Symbol =", exchSym)
            instruments.append(final_data)
    instruments = sorted(instruments, key=lambda d:d['Exchange-Symbol'])
    print(instruments)


if __name__ == "__main__":
    gemini_instrument_loader()
