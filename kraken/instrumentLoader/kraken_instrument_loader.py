#!/usr/bin/python3
import requests
import json

def printInstruments(instruments:list):
    print("[")
    for instrument in instruments:
        print(f"\t{instrument},")
    print("\b]")

def kraken_instrument_loader():
    instruments = []
    # URL to fetch symbols from Kraken exchange
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
        # print("Appending Symbol =", exchSym)
        # print("Details = ", final_data)
        instruments.append(final_data)
    printInstruments(instruments)
    # instruments = sorted(instruments, key=lambda d:d['Exchange-Symbol'])
    # print(instruments)
    
if __name__ == "__main__":
    kraken_instrument_loader()