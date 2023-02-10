#!/usr/bin/python3
import requests
import json


# URL to fetch symbols from Kraken exchange
def kraken_instrument_loader():
    instruments = []
    api_url = "https://api.kraken.com/0/public/AssetPairs"
    response = requests.get(api_url)
    symbols = response.json()
    for symbol, data in symbols['result'].items():
        if data['altname'].endswith('.d'):
            continue
        exchSym = symbol
        baseCcy = data['base']
        quoteCcy = data['quote']
        tickSize = data['lot']
        final_data = {
            "Exchange-Symbol":exchSym,
            "QPT-Symbol":baseCcy+quoteCcy,
            "Quote-Currency":quoteCcy,
            "Root-Symbol":baseCcy,
            "Tick-Size":tickSize
        }
        print("Appending Symbol =", exchSym)
        print("Details = ", final_data)
        instruments.append(final_data)
    instruments = sorted(instruments, key=lambda d:d['Exchange-Symbol'])
    print(instruments)
    
if __name__ == "__main__":
    kraken_instrument_loader()