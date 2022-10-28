#!/usr/bin/python3
import requests

#sandbox api url
api_url='https://api-public.sandbox.exchange.coinbase.com/'

def gdax_instrument_loader():
    response = requests.get('https://api-public.sandbox.exchange.coinbase.com/products')
    if response.status_code != 200:
        raise Exception("Failed To Fetch Data From Exchange")
    
    instruments = []
    for symbols in response.json():
        exchSymbol = symbols['id']
        qptContract = exchSymbol.replace("-", "")
        # rootSymbol = qptContract.replace('USD', '')
        baseCcy = symbols['base_currency']
        quoteCcy = symbols['quote_currency']    
        tickSize = symbols['quote_increment']
        final_data = {
            "Exchange-Symbol":exchSymbol,
            "QPT-Symbol":qptContract,
            "Quote-Currency":quoteCcy,
            "Root-Symbol":baseCcy,
            "Tick-Size":tickSize
        }
        print(final_data)
        instruments.append(final_data)
    print()
    print(instruments)

if __name__=="__main__":
    gdax_instrument_loader()
