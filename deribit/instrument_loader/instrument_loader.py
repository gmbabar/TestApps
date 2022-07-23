#!/usr/bin/python3

import requests
import datetime


def loadExchangeContracts(currency):
    asset_classes = ['future', 'option']
    for kind in asset_classes:
        counter = 0
        base_url = 'https://www.deribit.com'
        target = 'api/v2/public/get_instruments'
        full_url = '{0}/{1}?currency={2}&expired=false&kind={3}'.format(base_url, target, currency, kind)
        resp = requests.get(url=full_url)
        data = resp.json()
        if 'result' in data:
            for val in data['result']:
                print("kind : %s" % (val['kind']))
                expDtm = datetime.datetime.fromtimestamp(val['expiration_timestamp'] / 1000)
                expStr = "%.02d%.02d%.02d" % (expDtm.year % 2000, expDtm.month, expDtm.day)
                baseCcy = val['base_currency']
                quoteCcy = val['quote_currency']
                counterCcy = val['counter_currency']
                print("quote_currency : %s" % (val['quote_currency']))
                if kind == 'option':
                    print("strike : %s" % (val['strike']))
                    print("option_type : %s" % (val['option_type']))
                    optionType = 'C' if val['option_type'].lower() == 'call' else 'P'
                    print("Symbol: R_I_{0}_{1}_{2}_{3}_{4}".format(baseCcy, counterCcy, expStr, int(val['strike']), optionType))
                elif val['instrument_name'].lower().count("perpetual"):
                    print("future_type : %s" % (val['future_type']))
                    instType = "I" if val['future_type'].lower() == "reversed" else "T"
                    print("Symbol: R_{0}_{1}_{2}_SWAP".format(instType, baseCcy, quoteCcy))
                else:
                    print("future_type : %s" % (val['future_type']))
                    instType = "I" if val['future_type'].lower() == "reversed" else "T"
                    print("Symbol: R_{0}_{1}_{2}_{3}".format(instType, baseCcy, quoteCcy, expStr))
                print("base_currency : %s" % baseCcy)
                print("quote_currency : %s" % quoteCcy)
                print("counter_currency : %s" % (val['counter_currency']))
                print("settlement_currency : %s" % (val['settlement_currency']))
                print("instrument_id : %s" % (val['instrument_id']))
                print("instrument_name : %s" % (val['instrument_name']))  # exchange symbol
                print("expiration_timestamp : %s" % (val['expiration_timestamp']))
                print("expiration : %s" % expStr)
                print("---------------------------------------------------------")
                counter += 1
        print("Processed {0} symbols for {1}/{2}".format(counter, currency, kind))
        print("---------------------------------------------------------")


if __name__ == "__main__":
    url = 'https://www.deribit.com/api/v2/public/get_currencies'
    response = requests.get(url=url)
    data = response.json() # Check the JSON Response Content documentation below
    if 'result' in data:
        for val in data['result']:
            print("currency : %s" % (val['currency']))
            loadExchangeContracts(val['currency'])



