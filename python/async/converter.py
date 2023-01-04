import json

if __name__=="__main__":
    data = {"order_id":"106817811","id":"106817811","symbol":"btcusd","exchange":"gemini","avg_execution_price":"9500",
            "side":"sell","type":"exchange limit","timestamp":"1547220404","timestampms":1547220404836,"is_live":True,
            "is_cancelled":False,"is_hidden":False,"was_forced":False,"executed_amount":"0","remaining_amount":"1",
            "client_order_id":"ce5gpl22","options":["maker-or-cancel"],"price":"9500","original_amount":"1"}
    print(json.dumps(data))
    # pass