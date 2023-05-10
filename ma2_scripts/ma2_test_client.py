#!/usr/bin/python3

'''
https://docs.python.org/3/library/asyncio-protocol.html
'''
import asyncio
import argparse
import json
import time

from tomlkit import string

# some helper functions.

def check_type_int(json_data, value) -> bool:
    if value not in json_data:
        return False
    if type(json_data[value]) != int:
        return False
    return True

def check_type_str(json_data, value) -> bool:
    if value not in json_data:
        return False
    if type(json_data[value]) != str:
        return False
    return True

def check_type_number_in_string(json_data, value) -> bool:
    if not check_type_str(json_data, value):
        return False
    try:
        int(json_data[value])
    except:
        return False
    return True

def validate_number_range(number, min, max) -> bool:
    if number < min or number > max:
        return False
    return True

def validate_string_values(token, acceptable_values) -> bool:
    if token not in acceptable_values:
        return False
    return True


def build_heartbeat(msgId) -> str:
    return json.dumps({"id":msgId,"type":"hrbt"})

def build_helo(msgId: str, endpoint: str) -> str:
    return json.dumps({"id":msgId,"type":"helo","clid":"clt123","exid":endpoint,"init_data":"s"})

def build_new_order(msgId: str, clt_oid: str, endpoint: str, symbol: str) -> str:
    return json.dumps({"id":msgId,"type":"ornw","ord_cl_id":clt_oid,"cl_account_id":endpoint,"ord_type":"L",
                      "symbol":symbol,"amount":0.1,"price":16500,"post_only":1,"hidden":0,
                      "trader_id":"trd123","strategy":"trex","tif":"GTC","min_amount":0.0001})

def build_cancel(msgId: str, oid: str, cancelType = "C") -> str:
    if cancelType.upper() == "C":
        return json.dumps({"id":msgId,"type":"orcn","ord_cl_id":oid,"ord_ex_id":"exchOid123","can_id":54321})
    else:
        return json.dumps({"id":msgId,"type":"orcn","ord_ex_id":oid,"can_id":54322})

def build_pair_data(msgId) -> str:
    return json.dumps({"id":msgId,"type":"pdrq"})

def build_open_orders(msgId) -> str:
    return json.dumps({"id":msgId,"type":"oprq"})

def build_balance_req(msgId) -> str:
    return json.dumps({"id": msgId,"type":"blrq"})

TIMEOUT = 1 #seconds.

class Ma2ClientProtocol(asyncio.Protocol):
    msgId = 0
    errors = []
    warnings = []
    # poor man's state-machine
    appl_init = True
    helo_sent = False
    hrbt_sent = False
    pdrq_sent = False
    ornw_sent = False
    oprq_sent = False
    blrq_sent = False
    orcn_sent = False
    all_sent = False

    helo_recv = False
    pdrp_recv = False
    ornw_recv = False
    orrj_recv = False
    oprp_recv = False
    blrp_recv = False
    orcn_recv = False
    clrj_recv = False
    orst_recv = False
    ack_recv = False
    err_recv = False
    ack_counter = 0
    orst_counter = 0
    ornw_counter = 0
    order_id = round(time.time())
    exchId = ""
    # order_id = 'fixed_1234'     # To test duplicate order_id, uncomment and run twice

    def __init__(self, on_con_lost, endpoint, symbol):
        self.on_con_lost = on_con_lost
        self.loop = asyncio.get_running_loop()
        self.timeout_handle = self.loop.call_later(
            TIMEOUT, self._send_next,
        )
        self.endpoint = endpoint
        self.symbol = symbol

    # {
    #   "id"    : <number>,
    #   "type"  : "exst",
    #   "ts"    : <number>,
    #   "exid"  : <string>,
    #   "code"  : <number>,
    #   "msg"   : <string>
    # }
    def parse_exch_status(self, json):
        if 'id' not in json:
            error = "Missing 'id' in exchange state (exst)"
            if error not in self.errors:
                self.errors.append(error)
        if 'exid' not in json:
            error = "Missing 'exid' in exchange state (exst)"
            if error not in self.errors:
                self.errors.append(error)
        else:
            exid = json['exid']
            if exid.upper() != self.endpoint:
                self.errors.append(f"Invalid Exchange ID 'exid' exchange state (exst)', expected '{self.endpoint}', received '{exid}'")
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in exchange state (exst), expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'code' not in json:
            error = "Missing 'code' in exchange state (exst)"
            if error not in self.errors:
                self.errors.append(error)
        code = json['code']
        if check_type_number_in_string(json, 'code'):
            code = int(code)
        # else:
            # self.errors.append(f"Invalid 'code' type In 'exst', expected type: number-in-string, received: {type(code)} - {code}")
            # if not check_type_int(json, 'code'):
                # return
        # if code > 3 or code < 0:
            # self.errors.append(f"Invalid 'code' Received In 'exst', expected: 0 - 3, received: {code}")

    # {
    #   "id"              : <number>,
    #   "type"            : "pdrp",
    #   "ts"              : <number>,
    #   "exid"            : <string>,
    #   "pairs"           :
    #    [
    #       {
    #         "base_curr"      : <string>,
    #         "quote_curr"     : <string>,
    #         "trading_symbol" : <string>,
    #         "tick_size"      : <number>,
    #         "tick_unit"      : <number>,
    #         "step_size"      : <number>,
    #         "min_size_base"  : <number>,
    #         "max_size_base"  : <number>,
    #         "min_size_quote" : <number>,
    #         "max_size_quote" : <number>,
    #         "mult"           : <number>,
    #         "exp_date"       : <string>
    #       },
    #       ...
    #    ]
    # }
    def parse_pair_data(self, json):
        if 'id' not in json:
            error = "Missing 'id' in pairs data (pdrp)"
            if error not in self.errors:
                self.errors.append(error)
        if 'exid' not in json:
            error = "Missing 'exid' in pairs data (pdrp)"
            if error not in self.errors:
                self.errors.append(error)
        if 'pairs' not in json:
            error = "Missing 'pairs' in pairs data (pdrp)"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in pairs data (pdrp), expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        exid = json['exid']
        pairs = json['pairs']
        if exid.upper() != self.endpoint:
            self.errors.append(f"Invalid exchange ID {exid} received in pairs data (pdrp)")
        symbols = set()
        for pair_data in pairs:
            if 'base_curr' not in pair_data:
                error = "Missing 'base_curr' in pairs data (pdrp)"
                if error not in self.errors:
                    self.errors.append(error)
            if 'quote_curr' not in pair_data:
                error = "Missing 'quote_curr' in pairs data (pdrp)"
                if error not in self.errors:
                    self.errors.append(error)
            if 'tick_size' not in pair_data:
                error = "Missing 'tick_size' in pairs data (pdrp)"
                if error not in self.errors:
                    self.errors.append(error)
            if 'mult' not in pair_data:
                error = "Missing 'mult' in pairs data (pdrp)"
                if error not in self.errors:
                    self.errors.append(error)
            if 'trading_symbol' not in pair_data:
                error = "Missing 'trading_symbol' in pairs data (pdrp)"
                if error not in self.errors:
                    self.errors.append(error)
                continue
            trading_symbol = pair_data['trading_symbol']
            if not check_type_str(pair_data, 'trading_symbol'):
                error = f"Invalid 'trading_symbol' type {type(trading_symbol)} in pairs data (pdrp)"
                if error not in self.errors:
                    self.errors.append(error)
                continue
            if trading_symbol in symbols and trading_symbol.find:
                error = 'Duplicate symbol ' + trading_symbol
                if error not in self.errors:
                    self.errors.append(error)
            symbols.add(pair_data['trading_symbol'])
        print(f'Received {len(pairs)} Pairs In PairData Report')


    # {
    #     "id"    : <number>,
    #     "type"  : "ack",
    #     "ts"    : <number>,
    #     "refid" : <number>
    # }
    def parse_ack(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'ack' message"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in 'ack', expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'ack' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'refid' not in json:
            error = "Missing 'id' in 'ack' message"
            if error not in self.errors:
                self.errors.append(error)



    # {
    #     "id"    : <number>,
    #     "type"  : "err",
    #     "ts"    : <number>,
    #     "refid" : <number>,
    #     "code"  : <number>,
    #     "msg"   : <string>
    # }
    def parse_error(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'err' message"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in 'err' msg, expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'err' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'refid' not in json:
            error = "Missing 'id' in 'err' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'msg' not in json:
            error = "Message Not Received in Err"
            if error not in self.errors:
                self.errors.append(error)
            else:
                self.errors.append("Multiple Error Message Received For Same Message")
        self.errors.append(f"Error Message: {json['msg']}")


    # {
    #     "id"        	: <number>,
    #     "type"      	: "orcr",
    #     "ts"          : <number>,
    #     "ord_cl_id" 	: <string>,
    #     "ord_ex_id" 	: <string>,
    #     "cl_id"       : <string>,
    #     "symbol"      : <string>,
    #     "amount"    	: <number>,
    #     "price"     	: <number>
    # }
    def parse_order_created(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'orcr' message"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in 'orcr' msg, expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'orcr' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'symbol' not in json:
            error = "Missing 'symbol' in 'orcr' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'amount' not in json:
            error = "Missing 'amount' in 'orcr' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'price' not in json:
            error = "Missing 'price' in 'orcr' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_cl_id' not in json:
            error = "Missing 'ord_cl_id' in 'orcr' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_ex_id' not in json:
            error = "Missing 'ord_ex_id' in 'orcr' message"
            if error not in self.errors:
                self.errors.append(error)
        self.exchId = json['ord_ex_id']
        # elif '@' not in json['ord_ex_id']:
        #     error = "Invalid 'ord_ex_id' in 'orcr' message"
        #     if error not in self.errors:
        #         self.errors.append(error)


    # {
    #     "id"        	: <number>,
    #     "type"      	: "orst",
    #     "ts"          : <number>,
    #     "ord_cl_id" 	: <string>,
    #     "ord_ex_id" 	: <string>,
    #     "status"    	: <string>,
    #     "symbol"      : <string>,
    #     "amount"    	: <number>,
    #     "price"     	: <number>,
    #     "status_text"	: <string>
    # }
    def parse_order_status(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'orst' message"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in 'orst' msg, expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'orst' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'symbol' not in json:
            error = "Missing 'symbol' in 'orst' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'amount' not in json:
            error = "Missing 'amount' in 'orst' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'price' not in json:
            error = "Missing 'price' in 'orst' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_cl_id' not in json:
            error = "Missing 'ord_cl_id' in 'orst' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_ex_id' not in json:
            error = "Missing 'ord_ex_id' in 'orst' message"
            if error not in self.errors:
                self.errors.append(error)
        # elif '@' not in json['ord_ex_id']:
        #     error = "Invalid 'ord_ex_id' in 'orst' message"
        #     if error not in self.errors:
        #         self.errors.append(error)


    # {
    #     "id"        	: <number>,
    #     "type"      	: "ordn",
    #     "ts"          : <number>,
    #     "ord_cl_id" 	: <string>,
    #     "ord_ex_id" 	: <string>,
    #     "symbol"      : <string>,
    #     "can_id"      : <number>
    # }
    def parse_order_done(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'ordn' message"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in 'ordn' msg, expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'ordn' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_cl_id' not in json:
            error = "Missing 'ord_cl_id' in 'ordn' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_ex_id' not in json:
            error = "Missing 'ord_ex_id' in 'ordn' message"
            if error not in self.errors:
                self.errors.append(error)
        # elif '@' not in json['ord_ex_id']:
        #     error = "Invalid 'ord_ex_id' in 'ordn' message"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'can_id' not in json:
            error = "Missing 'can_id' in 'ordn' message"
            if error not in self.errors:
                self.errors.append(error)


    # {
    #     "id"        	: <number>,
    #     "type"      	: "orrj",
    #     "ts"          : <number>,
    #     "ord_cl_id" 	: <number>,
    #     "symbol"      : <string>,
    #     "reason"      : <string>,
    #     "code"        : <string>
    # }
    def parse_order_reject(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'orrj' message"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in 'orrj' msg, expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'orrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_cl_id' not in json:
            error = "Missing 'ord_cl_id' in 'orrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'symbol' not in json:
            error = "Missing 'symbol' in 'orrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'reason' not in json:
            error = "Missing 'reason' in 'orrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'reason' not in json:
            error = "Missing 'reason' in 'orrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'code' not in json:
            error = "Missing 'code' in 'orrj' message"
            if error not in self.errors:
                self.errors.append(error)


    # {
    #     "id"        	: <number>,
    #     "type"      	: "clrj",
    #     "ts"          : <number>,
    #     "ord_cl_id" 	: <string>,
    #     "ord_ex_id" 	: <string>,
    #     "reason"      : <string>,
    #     "can_id"      : <number>,
    #     "code"        : <string>
    # }
    def parse_cancel_reject(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'clrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if not check_type_number_in_string(json, 'id'):
            error = f"Invalid 'id' type in 'clrj' msg, expected: number-in-string, received: {type(json['id'])}"
            if error not in self.errors:
                self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'clrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_cl_id' not in json:
            error = "Missing 'ord_cl_id' in 'clrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_ex_id' not in json:
            error = "Missing 'ord_ex_id' in 'clrj' message"
            if error not in self.errors:
                self.errors.append(error)
        elif '@' not in json['ord_ex_id']:
            error = "Invalid 'ord_ex_id' in 'clrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'reason' not in json:
            error = "Missing 'reason' in 'clrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'can_id' not in json:
            error = "Missing 'can_id' in 'clrj' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'code' not in json:
            error = "Missing 'code' in 'clrj' message"
            if error not in self.errors:
                self.errors.append(error)


    # {
    #     "id"         : <number>,
    #     "type"       : "blrp",
    #     "ts"         : <number>,
    #     "account_id" : <number>,
    #     "bals"    : [{ <curr1> : <number> },...],
    #     "bals_ex" : { { <curr1> : {<name1> : <value1>,...}, ...}, ...}
    # }
    def parse_balance_report(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'blrp' message"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in 'blrp' msg, expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'blrp' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'account_id' not in json:
            error = "Missing 'account_id' in 'blrp' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'bals' not in json:
            error = "Missing 'bals' in 'blrp' message"
            if error not in self.errors:
                self.errors.append(error)


    # {
    #     "id"              : <number>,
    #     "type"            : "oprp",
    #     "ts"              : <number>,
    #     "exid"            : <string>,
    #     "account_id"      : <number>,
    #     "open_orders"     : <number>,
    #     "orders"          :
    #     [
    #     {
    #         "ord_ex_id"      : <string>,
    #         "cl_id"          : <string>,
    #         "symbol"         : <string>,
    #         "status"         : <string>,
    #         "orig_amount"    : <string>,
    #         "curr_amount"    : <string>,
    #         "price"          : <string>,
    #         "client_id"      : <string>
    #     },
    #     ...
    #     ]
    # }
    def parse_open_order_report(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'oprp' message"
            if error not in self.errors:
                self.errors.append(error)
        # if not check_type_number_in_string(json, 'id'):
        #     error = f"Invalid 'id' type in 'oprp' msg, expected: number-in-string, received: {type(json['id'])}"
        #     if error not in self.errors:
        #         self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'oprp' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'account_id' not in json:
            error = "Missing 'account_id' in 'oprp' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'orders' not in json:
            error = "Missing 'orders' in 'oprp' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'open_orders' not in json:
            error = "Missing 'open_orders' in 'oprp' message"
            if error not in self.errors:
                self.errors.append(error)
        elif not check_type_number_in_string(json, 'open_orders'):
            error = f"Invalid 'open_orders' type in 'oprp' msg, expected: number-in-string, received: {type(json['open_orders'])}"
            if error not in self.errors:
                self.errors.append(error)
        elif int(json['open_orders']) > 0:
            orders = json['orders']
            if int(json['open_orders']) != len(orders):
                error = "Mismatched 'open_orders' count with 'order' array in 'oprp' message"
                if error not in self.errors:
                    self.errors.append(error)
            for order in orders:
                if 'ord_ex_id' not in order:
                    error = "Missing 'ord_ex_id' in 'oprp' message"
                    if error not in self.errors:
                        self.errors.append(error)
                # elif '@' not in order['ord_ex_id']:
                #     error = "Invalid 'ord_ex_id' in 'oprp' message"
                #     if error not in self.errors:
                #         self.errors.append(error)
                if 'cl_id' not in order:
                    error = "Missing 'cl_id' in 'oprp' message"
                    if error not in self.errors:
                        self.errors.append(error)
                if 'symbol' not in order:
                    error = "Missing 'symbol' in 'oprp' message"
                    if error not in self.errors:
                        self.errors.append(error)
                if 'status' not in order:
                    error = "Missing 'status' in 'oprp' message"
                    if error not in self.errors:
                        self.errors.append(error)
                if 'orig_amount' not in order:
                    error = "Missing 'orig_amount' in 'oprp' message"
                    if error not in self.errors:
                        self.errors.append(error)
                if 'curr_amount' not in order:
                    error = "Missing 'curr_amount' in 'oprp' message"
                    if error not in self.errors:
                        self.errors.append(error)
                if 'price' not in order:
                    error = "Missing 'price' in 'oprp' message"
                    if error not in self.errors:
                        self.errors.append(error)
                if 'client_id' not in order:
                    warning = "Missing 'client_id' in 'oprp' message"
                    if warning not in self.warnings:
                        self.warnings.append(warning)



    # {
    #     "id"            : <number>,
    #     "type"          : "trex",
    #     "ts"            : <number>,
    #     "exec_id"       : <number>,
    #     "ord_cl_id"     : <string>,
    #     "ord_ex_id"     : <string>,
    #     "symbol"        : <string>,
    #     "amount"        : <number>,
    #     "price"         : <number>,
    #     "liquidity"     : <string>,
    #     "fee"           : <string>,
    #     "fee_currency"  : <string>,
    #     "bals"    : [{ <curr1> : <number> },...],
    #     "bals_ex" : { { <curr1> : {<name1> : <value1>,...}, ...}, ...}
    # }
    def parse_trace_execution(self, json):
        if 'id' not in json:
            error = "Missing 'id' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if not check_type_number_in_string(json, 'id'):
            error = f"Invalid 'id' type in 'trex' msg, expected: number-in-string, received: {type(json['id'])}"
            if error not in self.errors:
                self.errors.append(error)
        if 'ts' not in json:
            error = "Missing 'ts' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'exec_id' not in json:
            error = "Missing 'exec_id' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_cl_id' not in json:
            error = "Missing 'ord_cl_id' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'ord_ex_id' not in json:
            error = "Missing 'ord_ex_id' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        elif '@' not in json['ord_ex_id']:
            error = "Invalid 'ord_ex_id' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'symbol' not in json:
            error = "Missing 'symbol' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'amount' not in json:
            error = "Missing 'amount' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'price' not in json:
            error = "Missing 'price' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'liquidity' not in json:
            error = "Missing 'liquidity' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'fee' not in json:
            error = "Missing 'fee' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'fee_currency' not in json:
            error = "Missing 'fee_currency' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'bals' not in json:
            error = "Missing 'bals' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)
        if 'bals_ex' not in json:
            error = "Missing 'bals_ex' in 'trex' message"
            if error not in self.errors:
                self.errors.append(error)


    def connection_made(self, transport):
        print('Connection made.')
        self.transport = transport
        # self._send_msg()


    def data_received(self, data):
        msg_str = data.decode()
        # print(msg_str)
        if msg_str[-1] != '\n':
            newline_error = 'Missing new line (\\n) at the end of msg(s)'
            if newline_error not in self.errors:
                self.errors.append(newline_error)
        if msg_str.find('}{'):
            msg_str = msg_str.replace('}{', '}\n{')
        msgs = msg_str.strip().split('\n')
        for msg in msgs:
            print(f'Received: {msg}')

            try:
                json_data = json.loads(msg)
            except:
                error = f"_Critical: Malformed json msg received {msg}"
                if error not in self.errors:
                    self.errors.append(error)
                return

            if 'type' not in json_data:
                self.errors.append("_Critical: Missing 'type' in received MA2 message.")
                return
            if self.appl_init:
                self.errors.append("Message '" + json_data['type'] + "' received even before 'helo' message")
                if json_data['type'] == 'exst':
                    self.parse_exch_status(json_data)
                elif json_data['type'] == 'blrp':
                    self.parse_balance_report(json_data)
                elif json_data['type'] == 'pdrp':
                    self.parse_pair_data(json_data)
                elif json_data['type'] == 'ack':
                    self.parse_ack(json_data)
                elif json_data['type'] == 'orcr':
                    self.parse_order_created(json_data)
                elif json_data['type'] == 'orst':
                    self.parse_order_status(json_data)
                elif json_data['type'] == 'ordn':
                    self.parse_order_done(json_data)
                elif json_data['type'] == 'orrj':
                    self.parse_order_reject(json_data)
                elif json_data['type'] == 'clrj':
                    self.parse_cancel_reject(json_data)
                elif json_data['type'] == 'oprp':
                    self.parse_open_order_report(json_data)
                elif json_data['type'] == 'trex':
                    self.parse_trace_execution(json_data)

                return
            if json_data['type'] == 'err':
                self.parse_error(json_data)
                self.err_recv = True
                return
            self.err_recv = False
            if json_data['type'] == 'exst':
                if self.helo_sent:
                    self.helo_recv = True
                    self.parse_exch_status(json_data)

            if self.pdrq_sent:
                if json_data['type'] == 'pdrp':
                    self.pdrp_recv = True
                    self.parse_pair_data(json_data)
                else:
                    self.errors.append("No Pair Data Report Received")
            else:
                if json_data['type'] == 'pdrp':
                    self.parse_pair_data(json_data)

            if self.blrq_sent:
                if json_data['type'] == 'blrp':
                    self.blrp_recv = True
                    self.parse_balance_report(json_data)
                else:
                    self.errors.append("No Balance Report Received")
            else:
                if json_data['type'] == 'blrp':
                    self.parse_balance_report(json_data)

            if self.oprq_sent:
                if json_data['type'] == 'oprp':
                    self.oprp_recv = True
                    self.parse_open_order_report(json_data)
                else:
                    self.errors.append("No Open Order Report Received")
            else:
                if json_data['type'] == 'oprp':
                    self.parse_open_order_report(json_data)

            if self.ornw_sent:
                if json_data['type'] == 'ack':
                    self.ack_recv = True
                    self.parse_ack(json_data)
                elif json_data['type'] == 'orcr':
                    self.ornw_recv = True
                    self.parse_order_created(json_data)
                elif json_data['type'] == 'orrj':
                    self.orrj_recv = True
                    self.parse_order_reject(json_data)
                else:
                    self.errors.append("No Order Creation Message Received For New Order")
            elif json_data['type'] == 'orcr':
                self.warnings.append("Received New Order (orcr) For Unknown Order")
            elif json_data['type'] == 'orrj':
                self.warnings.append("Received Order Reject (orrj) For Unknown Order")

            if self.orcn_sent:
                if json_data['type'] == 'ack':
                    self.ack_recv = True
                    self.parse_ack(json_data)
                elif json_data['type'] == 'orst':
                    self.orst_recv = True
                    self.parse_order_status(json_data)
                elif json_data['type'] == 'ordn':
                    self.orcn_recv = True
                    self.parse_order_done(json_data)
                elif json_data['type'] == 'clrj':
                    self.clrj_recv = True
                    self.parse_order_done(json_data)
                else:
                    self.errors.append("No Cancel Order Response Received"+json_data['type'])
            elif json_data['type'] == 'orst':
                self.warnings.append("Received Order State (orst) For Unknown Order")
            elif json_data['type'] == 'ordn':
                self.warnings.append("Received Order Done (ordn) For Unknown Order")
            elif json_data['type'] == 'clrj':
                self.warnings.append("Received Cancel Reject (clrj) For Unknown Order")


    def connection_lost(self, exc):
        print('The server closed the connection')
        self.on_con_lost.set_result(True)
        if len(self.errors) != 0:
            print(f"-> \33[31mFound {len(self.errors)} Errors \U0001F61F")
            self.errors.sort()
            for errors in self.errors:
                print(f'\t{errors}')
        if len(self.warnings) != 0:
            print(f"-> \33[33mFound {len(self.warnings)} Warnings \U0001F61F")
            self.warnings.sort()
            for warning in self.warnings:
                print(f'\t{warning}')
        else:
            print("-> \33[32mNo Errors Were Found \U0001F60A")


    def _send_next(self):
        print('Timeout invoked, sending next mesg.')
        self.msgId +=1
        if self.appl_init:  # first message.
            self.message = build_helo(self.msgId, self.endpoint)
            self.appl_init = False
            self.helo_sent = True
        elif self.helo_sent:
            self.message = build_heartbeat(self.msgId)
            if not self.helo_recv:
                self.errors.append("No Helo Response Received")
            self.helo_sent = False
            self.hrbt_sent = True
        elif self.hrbt_sent:
            self.hrbt_sent = False
            self.message = build_pair_data(self.msgId)
            self.pdrq_sent = True
        elif self.pdrq_sent:
            self.pdrq_sent = False
            if not self.pdrp_recv:
                self.errors.append("No Pair Data Report Received")
            self.message = build_new_order(self.msgId, str(self.order_id), self.endpoint, self.symbol)
            self.ornw_sent = True
        elif self.ornw_sent and self.ornw_counter < 2:
            self.ornw_sent = False
            if self.orrj_recv:
                self.warnings.append("New order got rejected.")
            else:
                if not self.ack_recv:
                    self.errors.append("No Ack Received For New Order")
                if not self.ornw_recv:
                    self.errors.append("No New Order Created")
            self.ornw_counter +=1
            self.message = build_open_orders(self.msgId)
            self.oprq_sent = True
        elif self.oprq_sent:
            self.oprq_sent = False
            if not self.oprp_recv:
                self.errors.append("No Open Order Report Received")
            if self.ornw_recv and self.ornw_counter != 1:
                self.message = build_cancel(self.msgId, self.exchId, "E")
                self.orcn_sent = True
                self.ack_recv = False
            elif self.ornw_recv:
                self.message = build_cancel(self.msgId, str(self.order_id))
                self.orcn_sent = True
                self.ack_recv = False
            else:   # cancel n/a, so move next to balance request
                self.message = build_balance_req(self.msgId)
                self.blrq_sent = True
        elif self.orcn_sent:
            self.orcn_sent = False
            if self.clrj_recv:
                self.warnings.append("Cancel order got rejected.")
            else:
                if not self.ack_recv:
                    if not self.err_recv:
                        self.errors.append("No Ack Received For Cancel (orcn)")
                if not self.orst_recv:
                    if not self.err_recv:
                        self.errors.append("No Order State Received For Cancel (orcn)")
                if not self.orcn_recv:
                    if not self.err_recv:
                        self.errors.append("No Order Done Received For Cancel (orcn)")
            self.message = build_balance_req(self.msgId)
            self.blrq_sent = True
        elif self.blrq_sent:
            self.blrq_sent = False
            if not self.blrp_recv:
                self.errors.append("No Balance Report Received")
            self.order_id += 1
            self.message = build_new_order(self.msgId, str(self.order_id), self.endpoint, self.symbol)
            self.ornw_sent = True
            self.all_sent = True
        elif self.all_sent:
            print('All test messages are done. closing connection...')
            self.transport.close()
            return
        else:
            self.message = build_heartbeat(self.msgId)
            self.all_sent = True
        self._send_msg()
        # setup timeout
        self.timeout_handle = self.loop.call_later(
            TIMEOUT, self._send_next,
        )

    def _send_msg(self) -> None:
        if self.message[-1] != '\n':
            self.message += '\n'
        self.transport.write(self.message.encode())
        print('Data sent: {!r}'.format(self.message))


def init_argparse() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description='Test client for MA2 deribit implementation')
    # '-h' is reserved for help by argparse package, so '-s' is forced choice for host/server
    parser.add_argument('-s', '--host', help='server (host) name or ip address for MA2 server', required=True)
    parser.add_argument('-p', '--port', help='port number for MA2 server', required=True, type=int)
    parser.add_argument('-e', '--endpoint', help='Endpoint/exchID for MA2 exchange [DERI, GDAX]', required=True, type=str)
    parser.add_argument('-i', '--instrument', help='Instrument for MA2 exchange [BTC-PERPETUAL, BTC-USD]', required=True, type=str)
    # args = vars(parser.parse_args())
    return parser


async def main() -> None:
    # cmd line args parser
    parser = init_argparse()
    args = parser.parse_args()
    host = args.host
    port = args.port
    endpoint = args.endpoint
    symbol = args.instrument

    # Get a reference to the event loop for low-level APIs.
    loop = asyncio.get_running_loop()
    on_con_lost = loop.create_future()

    print ('Connecting client to ', host, ':', port)
    transport, protocol = await loop.create_connection(
        lambda: Ma2ClientProtocol(on_con_lost, endpoint, symbol),
        host, port)

    # Wait until the protocol signals that the connection
    # is lost and close the transport.
    try:
        await on_con_lost
    finally:
        transport.close()


if __name__ == '__main__':
    asyncio.run(main())

