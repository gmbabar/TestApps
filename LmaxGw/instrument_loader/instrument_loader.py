#!/usr/bin/python3
import requests

if __name__=='__main__':
    url = 'https://public-data-api.london-digital.lmax.com/v1/instruments/'
    result = requests.get(url)
    data = result.json()
    instruments = []
    for symbol in data:
        baseCcy = symbol['unit_of_measure']
        quoteCcy = symbol['currency']
        exchSymbol = symbol['symbol']
        tickSize = symbol['price_increment']
        finalData = {
                "Exchange-Symbol":exchSymbol,
                "QPT-Symbol":exchSymbol,
                "Quote-Currency":quoteCcy,
                "Root-Symbol":baseCcy,
                "Tick-Size":tickSize
            }
        instruments.append(finalData)
        print(finalData)