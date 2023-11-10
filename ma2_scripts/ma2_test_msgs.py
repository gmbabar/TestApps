#!/usr/bin/python3

'''
https://docs.python.org/3/library/asyncio-protocol.html
'''
import asyncio
import argparse
import json
import time
from enum import Enum, auto, unique

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

def build_new_order(msgId: str, clt_oid: str, endpoint: str, symbol: str, qty: str, price: str) -> str:
    return json.dumps({"id":msgId,"type":"ornw","ord_cl_id":clt_oid,"cl_account_id":endpoint,"ord_type":"L",
                      "symbol":symbol,"amount":qty,"price":price,"trader_id":"trd123",
                      "strategy":"trex","tif":"GTC","min_amount":0.0001})

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

TIMEOUT = 3 #seconds.

@unique
class Ma2ClientStates(Enum):
    appl_init = auto()
    helo_sent = auto()
    hrbt_sent = auto()
    pdrq_sent = auto()
    ornw_sent = auto()
    oprq_sent = auto()
    blrq_sent = auto()
    orcn_sent = auto()
    all_sent = auto()

class Ma2ClientProtocol(asyncio.Protocol):
    # Initialization
    orderId = round(time.time())
    exchId = ""
    # orderId = 'fixed_1234'     # To test duplicate orderId, uncomment and run twice
    msgId = 0
    outMsg = ""
    inMsg = ""
    state = Ma2ClientStates.appl_init

    def __init__(self, on_con_lost, endpoint, symbol, qty, price):
        self.on_con_lost = on_con_lost
        self.loop = asyncio.get_running_loop()
        self.timeout_handle = self.loop.call_later(
            TIMEOUT, self._send_next,
        )
        self.endpoint = endpoint
        self.symbol = symbol
        self.qty = qty
        self.price = price

    def connection_made(self, transport):
        print('Connection made.')
        self.transport = transport

    def data_received(self, data):
        msg_str = data.decode()
        # print(msg_str)
        json_data = {}
        if msg_str[-1] != '\n':
            print('\33[31mError: Missing new line (\\n) at the end of msg(s), saving until we receive full msg\33[0m')
            self.inMsg += msg_str   # keep saving until we receive complete message.
            return
        if msg_str.find('}{'):
            msg_str = msg_str.replace('}{', '}\n{')
        self.inMsg += msg_str
        msgs = self.inMsg.strip().split('\n')
        for msg in msgs:
            print(f'\33[35mReceived:\33[0m {msg}')
            try:
                json_data = json.loads(msg)
                try:
                    if 'type' in json_data and json_data['type'] == 'blrp':
                        if 'bals' in json_data:
                            bals = json_data['bals']
                            json_data['bals'] = sorted(bals, key=lambda k: list(k.keys())[0])
                        if 'bals_ex' in json_data:
                            bals_ex = json_data['bals_ex']
                            json_data['bals_ex'] = dict(sorted(bals_ex.items()))
                        print(f'\33[35mSorted Balances:\33[0m {json_data}')
                except Exception as e:
                    print(f'\33[31mException in blrp SORT: {e}\33[0m')
            except:
                error = f"\33[31mCritical Error: Malformed json msg received (showing 20 chars only): {msg[:20]} ...\33[0m"
                print(error)
                # return
        self.inMsg = ""
        if 'type' not in json_data:
            self.errors.append("_Critical: Missing 'type' in received MA2 message.")
            return
        if json_data['type'] == 'ack':
            return
        self._send_next()

    def connection_lost(self, exc):
        print('\33[31mThe server closed the connection\33[0m')
        self.on_con_lost.set_result(True)

    def _send_next(self):
        self.msgId +=1
        if self.state == Ma2ClientStates.appl_init:
            self.on_helo()
        elif self.state == Ma2ClientStates.helo_sent:
            self.on_hrbt_sent()
        elif self.state == Ma2ClientStates.hrbt_sent:
            self.on_pdrq_sent()
        elif self.state == Ma2ClientStates.pdrq_sent:
            self.on_ornw_sent()
        elif self.state == Ma2ClientStates.ornw_sent:
            self.on_oprq_sent()
        elif self.state == Ma2ClientStates.oprq_sent:
            self.on_blrq_sent()
        elif self.state == Ma2ClientStates.blrq_sent:
            self.on_orcn_sent()
        elif self.state == Ma2ClientStates.orcn_sent:
            self.on_all_sent()
        elif self.state == Ma2ClientStates.all_sent:
            self.on_all_sent()
        else:
            print("\33[31mError: Unknown state\33[0m")
            return
        # next state
        if (self.state.value < len(Ma2ClientStates)):
            self.state = Ma2ClientStates(self.state.value + 1)

    #  hello msg
    def on_helo(self):
        self.outMsg = build_helo(self.msgId, self.endpoint)
        self._send_msg()

    #  heartbeat msg
    def on_hrbt_sent(self):
        self.outMsg = build_heartbeat(self.msgId)
        self._send_msg()
        # no response for hrbt, so setup next send
        self.timeout_handle = self.loop.call_later(
            TIMEOUT, self._send_next,
        )

    #  pair_data request msg
    def on_pdrq_sent(self):
        self.outMsg = build_pair_data(self.msgId)
        self._send_msg()

    #  balance request msg
    def on_blrq_sent(self):
        self.outMsg = build_balance_req(self.msgId)
        self._send_msg()

    #  new order msg
    def on_ornw_sent(self):
        self.outMsg = build_new_order(self.msgId, str(self.orderId), self.endpoint, self.symbol, self.qty, self.price)
        self._send_msg()

    #  open order request msg
    def on_oprq_sent(self):
        self.outMsg = build_open_orders(self.msgId)
        self._send_msg()

    #  cancel request msg
    def on_orcn_sent(self):
        self.outMsg = build_cancel(self.msgId, str(self.orderId))
        self._send_msg()

    #  All Done
    def on_all_sent(self):
        print("\33[32mAll outMsgs are sent. closing connection...\033[0m")
        self.timeout_handle = self.loop.call_later(TIMEOUT, self.shutdown)

    def shutdown(self):
        print("\33[32mShutdown initiated.\033[0m")
        self.transport.close()

    def _send_msg(self) -> None:
        if self.outMsg[-1] != '\n':
            self.outMsg += '\n'
        self.transport.write(self.outMsg.encode())
        self.msgId +=1
        print('\33[34mData sent:\33[0m {!r}'.format(self.outMsg))


def init_argparse() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description='Test client for MA2 deribit implementation')
    # '-h' is reserved for help by argparse package, so '-s' is forced choice for host/server
    parser.add_argument('-s', '--host', help='server (host) name or ip address for MA2 server', required=True)
    parser.add_argument('-p', '--port', help='port number for MA2 server', required=True, type=int)
    parser.add_argument('-e', '--endpoint', help='Endpoint/exchID for MA2 exchange [DERI, GDAX]', required=True, type=str)
    parser.add_argument('-i', '--instrument', help='Instrument for MA2 exchange [BTC-PERPETUAL, BTC-USD]', required=True, type=str)
    parser.add_argument('-q', '--quantity', help='Order Qty for MA2 exchange', required=True, type=str)
    parser.add_argument('-c', '--price', help='Order Price for MA2 exchange', required=True, type=float)
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
    qty = args.quantity
    price = args.price

    try:
        # Get a reference to the event loop for low-level APIs.
        loop = asyncio.get_running_loop()
        on_con_lost = loop.create_future()

        proto_factory = lambda: Ma2ClientProtocol(on_con_lost, endpoint, symbol, qty, price)
        print ('Connecting client to ', host, ':', port)
        transport, protocol = await loop.create_connection(
            proto_factory, host, port)

        # Wait until the protocol signals that the connection
        # is lost and close the transport.
        try:
            await on_con_lost
        finally:
            print('Closing transport')
            transport.close()
    except:
        print('Error: Connection failed')


# BINA example:
#   ./ma2_test_msgs.py -s localhost -p 12347 -e BINA -i BTCUSDT -q 0.001 -c 16500
# DERI example:
#   ./ma2_test_msgs.py -s localhost -p 12390 -e DERI -i BTC-PERPETUAL -q 100 -c 16500

if __name__ == '__main__':
    asyncio.run(main())

