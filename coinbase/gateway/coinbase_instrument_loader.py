#!/usr/bin/python3
import requests
import json

#sandbox api url
api_url='https://api-public.sandbox.exchange.coinbase.com/'

def gdax_instrument_loader():
    response = requests.get('https://api-public.sandbox.exchange.coinbase.com/products')
    if response.status_code != 200:
        raise Exception("Failed To Fetch Data From Exchange")
    
    instruments = []
    for symbols in response.json():
        if(symbols['status'] == 'online'):
            print(json.dumps(symbols))
            exchSymbol = symbols['id']
            # rootSymbol = qptContract.replace('USD', '')
            baseCcy = symbols['base_currency']
            quoteCcy = symbols['quote_currency']    
            tickSize = symbols['quote_increment']
            final_data = {
                "Exchange-Symbol":exchSymbol,
                "QPT-Symbol":baseCcy+quoteCcy,
                "Quote-Currency":quoteCcy,
                "Root-Symbol":baseCcy,
                "Tick-Size":tickSize
            }
            # print(final_data)
            instruments.append(final_data)
            print()
    print()
    instruments = sorted(instruments, key=lambda d:d['Exchange-Symbol'])
    # instruments.sort()
    print("[")
    for instrument in instruments:
        print(f"\t{instrument}")
    print("]")
    # print(instruments)

if __name__=="__main__":
    gdax_instrument_loader()
