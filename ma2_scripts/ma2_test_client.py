#!/usr/bin/python3

'''
https://docs.python.org/3/library/asyncio-protocol.html
'''
import asyncio, json

import argparse

def build_heartbeat(msgId):
    return json.dumps({"id":msgId,"type":"hrbt"})

def build_helo(msgId):
    return json.dumps({"id":msgId,"type":"helo","clid":"clt123","exid":"DERI","init_data":"s"})

def build_new_order(msgId):
    return json.dumps({"id":msgId,"type":"ornw","ord_cl_id":"abcd1234","cl_account_id":"DERI","ord_type":"L",
                      "symbol":"BTC-PERPETUAL","amount":100,"price":16500,"post_only":1,"hidden":0,
                      "trader_id":"trd123","strategy":"trex","tif":"GTC","min_amount":10})

def build_cancel(msgId):
    return json.dumps({"id":msgId,"type":"orcn","ord_cl_id":"abcd1234","ord_ex_id":"exchOid123","can_id":54321})

def build_pair_data(msgId):
    return json.dumps({"id":msgId,"type":"pdrq"})

def build_open_orders(msgId):
    return json.dumps({"id":msgId,"type":"oprq"})

def build_balance_req(msgId):
    return json.dumps({"id": msgId,"type":"blrq"})

TIMEOUT = 1 #seconds.

class Ma2ClientProtocol(asyncio.Protocol):
    msgId = 0
    errors = []
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
    oprp_recv = False
    blrp_recv = False
    orcn_recv = False
    ack_recv = False

    def __init__(self, on_con_lost):
        # self.message = build_helo(self.msgId)
        # self.helo_sent = True
        self.on_con_lost = on_con_lost
        self.loop = asyncio.get_running_loop()
        self.timeout_handle = self.loop.call_later(
            TIMEOUT, self._timeout,
        )


    # {
    #   "id"    : <number>,
    #   "type"  : "exst",
    #   "ts"    : <number>,
    #   "exid"  : <string>,
    #   "code"  : <number>,
    #   "msg"   : <string>
    # }
    def parse_exch_status(self, json):
        exid = json['exid']
        if exid.upper() != "DERI":
            self.errors.append("Invalid Exchange ID Received In 'exid' (hello reponse)")
        code = json['code']
        if int(code) > 3 or int(code) < 0:
            self.errors.append("Invalid Code Received In 'exid' (hello response)")

    # {
    #   "id"              : <number>,
    #   "type"            : "pdrp",
    #   "ts"              : <number>,
    #   "exid"            : <string>,
    #   "pairs"           :
    #     [
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
    #     ]
    # }
    def parse_pair_data(self, json):
        exid = json['exid']
        pairs = json['pairs']
        if exid.upper() != "DERI":
            self.errors.append("Invalid Exchange ID Received In Helo")
        print(f'Received {len(pairs)} Pairs In PairData Report')


    def parse_ack(self, json):
        pass

    def parse_order_created(self, json):
        pass

    def parse_order_reject(self, json):
        pass

    def parse_balance_report(self, json):
        pass

    def parse_open_order_report(self, json):
        pass

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
        msgs = (msg_str.strip()).split('\n')
        for ele in msgs:
            print(f'Received: {ele}')

            json_data = json.loads(ele)
            if self.appl_init:
                self.errors.append("'" + json_data['type'] + "' received even before 'helo' message")
                return

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
                    self.ornw_recv = True
                    self.ack_recv = True
                    self.parse_ack(json_data)
                elif json_data['type'] and not self.ack_recv:
                    self.errors.append("No Ack Received For New Order")
                elif json_data['type'] == 'orcr':
                    self.ornw_recv = True
                    self.parse_order_created(json_data)
                elif json_data['type'] == 'orrj':
                    self.parse_order_reject(json_data)
                else:
                    self.errors.append("No Order Creation Message Received For New Order")
            elif not self.ornw_sent and (json_data['type'] == 'ack' or json_data['type'] == 'orrj'):
                self.errors.append("Received New Order Without Request")


    def connection_lost(self, exc):
        print('The server closed the connection')
        self.on_con_lost.set_result(True)
        if len(self.errors) != 0:
            print(f"-> \33[31mFound {len(self.errors)} Errors \U0001F61F")
            for errors in self.errors:
                print(f'\t{errors}')
        else:
            print("-> \33[32mNo Errors Were Found \U0001F60A")

    def _timeout(self):
        print('Timeout invoked.')
        self.msgId +=1
        if self.appl_init:  # first message.
            self.message = build_helo(self.msgId)
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
            self.message = build_new_order(self.msgId)
            if not self.pdrp_recv:
                self.errors.append("No Pair Data Report Received")
            self.ornw_sent = True
        elif self.ornw_sent:
            self.ornw_sent = False
            if not self.ornw_recv:
                self.errors.append("No New Order Created")
            self.message = build_open_orders(self.msgId)
            self.oprq_sent = True
        elif self.oprq_sent:
            self.oprq_sent = False
            if not self.oprp_recv:
                self.errors.append("No Open Order Report Received")
            if self.ornw_recv:
                self.message = build_cancel(self.msgId)
                self.orcn_sent = True
            else:   # cancel n/a, so move next to balance request
                self.message = build_balance_req(self.msgId)
                self.blrq_sent = True
        elif self.orcn_sent:
            self.orcn_sent = False
            if not self.orcn_recv:
                self.errors.append("No Order Cancel Message Received For orcn")
            self.message = build_balance_req(self.msgId)
            self.blrq_sent = True
        elif self.blrq_sent:
            self.blrq_sent = False
            if not self.blrp_recv:
                self.errors.append("No Balance Report Received")
            self.message = build_heartbeat(self.msgId)
            self.all_sent = True
        elif self.all_sent:
            print('All test messages are done. closing connection...')
            self.transport.close()
        else:
            self.message = build_heartbeat(self.msgId)
            self.all_sent = True
        self._send_msg()
        # setup timeout
        self.timeout_handle = self.loop.call_later(
            TIMEOUT, self._timeout,
        )

    def _send_msg(self) -> None:
        if self.message[-1] != '\n':
            self.message += '\n'
        self.transport.write(self.message.encode())
        print('Data sent: {!r}'.format(self.message))


def init_argparse() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description='Test client for MA2 deribit implementation')
    # '-h' is reserved for help by argparse package, so '-s' is forced choice for host/server
    parser.add_argument('-s', '--host', help='server (host) name or ip address of MA2 server', required=True)
    parser.add_argument('-p', '--port', help='port number for MA2 server', required=True, type=int)
    # args = vars(parser.parse_args())
    return parser


async def main() -> None:
    # cmd line args parser
    parser = init_argparse()
    args = parser.parse_args()
    host = args.host
    port = args.port

    # Get a reference to the event loop for low-level APIs.
    loop = asyncio.get_running_loop()
    on_con_lost = loop.create_future()

    print ('Connecting client to ', host, ':', port)
    transport, protocol = await loop.create_connection(
        lambda: Ma2ClientProtocol(on_con_lost),
        host, port)

    # Wait until the protocol signals that the connection
    # is lost and close the transport.
    try:
        await on_con_lost
    finally:
        transport.close()

asyncio.run(main())

