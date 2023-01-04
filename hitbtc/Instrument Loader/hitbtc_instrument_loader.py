#!/usr/bin/python3
import json
import time
import requests

api_url='https://api.hitbtc.com/api/3/public/symbol'

def printInstruments(instruments:list):
    print("[")
    for instrument in instruments:
        print(f"\t{instrument},")
    print("\b]")

def gemini_instrument_loader():
    spotInstruments = []
    futureInstruments = []
    response = requests.get(api_url)
    symbols = response.json()
    for symbol in symbols:
        exchSym = symbol
        symType = symbols[symbol]["type"]
        baseCcy = symbols[symbol]["base_currency"]
        quoteCcy = symbols[symbol]["quote_currency"]
        tickSize = symbols[symbol]["tick_size"]
        if(symType == "spot"):
            final_data = {
                "Exchange-Symbol":exchSym,
                "QPT-Symbol":baseCcy+quoteCcy,
                "Type":symType,
                "Quote-Currency":quoteCcy,
                "Root-Symbol":baseCcy,
                "Tick-Size":tickSize
            }
            spotInstruments.append(json.dumps(final_data))
        else:
            final_data = {
                "Exchange-Symbol":exchSym,
                "QPT-Symbol":quoteCcy,
                "Type":symType,
                "Quote-Currency":quoteCcy,
                "Root-Symbol":baseCcy,
                "Tick-Size":tickSize
            }
            futureInstruments.append(json.dumps(final_data))
    printInstruments(spotInstruments)
    printInstruments(futureInstruments)


if __name__ == "__main__":
    gemini_instrument_loader()

