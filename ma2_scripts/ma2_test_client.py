#!/usr/bin/python3

'''
https://docs.python.org/3/library/asyncio-protocol.html
'''
import asyncio, json

from sympy import false

def build_heartbeat(msgId):
    return json.dumps({"id":msgId,"type":"hrbt"})

def build_helo(msgId):
    return json.dumps({"id":msgId,"type":"helo","clid":"clt123","exid":"DERI","init_data":"s"})

def build_newOrder(msgId):
    return json.dumps({"id":msgId,"type":"ornw","ord_cl_id":"abcd1234","cl_account_id":"DERI","ord_type":"L","symbol":"BTC-PERPETUAL","amount":100,"price":16500,"post_only":1,"hidden":0,"trader_id":"trd123","strategy":"trex","tif":"GTC","min_amount":10})

def build_cancel(msgId):
    return json.dumps({"id":msgId,"type":"orcn","ord_cl_id":"abcd1234","ord_ex_id":"exchOid123","can_id":54321})

def build_pairData(msgId):
    return json.dumps({"id":msgId,"type":"pdrq"})

def build_openOrders(msgId):
    return json.dumps({"id":msgId,"type":"oprq"})

def build_balanceReq(msgId):
    return json.dumps({"id": msgId,"type":"blrq"})



TIMEOUT = 1 #seconds.

class EchoClientProtocol(asyncio.Protocol):
    msgId = 0
    Errors = []
    heloSent = False
    hrbtSent = False
    pdrqSent = False
    ornwSent = False
    oprqSent = False
    blrqSent = False
    orcnSent = False

    allSent = False

    def __init__(self, on_con_lost):
        self.message = build_helo(self.msgId)
        self.heloSent = True
        self.on_con_lost = on_con_lost
        self.loop = asyncio.get_running_loop()
        self.timeout_handle = self.loop.call_later(
            TIMEOUT, self._timeout,
        )

    def connection_made(self, transport):
        print('Connection made.')
        self.transport = transport
        self._send_msg()

    def data_received(self, data):
        data = (data.decode().strip()).split('\n')
        for ele in data:
            print(f'Received: {ele}')

            jData = json.loads(ele)['type']

            if jData == 'err':
                self.Errors.append(json.loads(ele)['msg'])

            if self.heloSent and jData != 'exst':
                if jData != 'blrp' or jData != 'pdrp':
                    self.Errors.append("No Response Recieved For helo")
            elif self.pdrqSent and jData != 'pdrp':
                self.Errors.append("No Response Received For pdrq")
            elif self.ornwSent and jData != 'ack':
                if jData == 'orrj' and json.loads(ele)['reason'] not in self.Errors:
                    self.Errors.append(json.loads(ele)['reason'])
                if jData != 'orcr' and "No Order Created On ornw" not in self.Errors:
                    self.Errors.append("No Order Created On ornw")
                    return
            elif self.oprqSent and jData != 'oprp':
                self.Errors.append("No Response Received For oprq")
            elif self.blrqSent and jData != 'blrp':
                self.Errors.append("No Response Received For blrq")
            elif self.orcnSent and jData != 'ack':
                if jData == 'clrj' and json.loads(ele)['reason'] not in self.Errors:
                    self.Errors.append(json.loads(ele)['reason'])
                elif self.orcnSent and jData != 'orst':
                    if self.orcnSent and jData != 'ordn' and 'No Order Canceled' not in self.Errors:
                        self.Errors.append("No Order Canceled")

    def connection_lost(self, exc):
        print('The server closed the connection')
        self.on_con_lost.set_result(True)
        if len(self.Errors) != 0:
            print(f"-> \33[31mFound {len(self.Errors)} Errors \U0001F61F")
            for errors in self.Errors:
                print(f'\t{errors}')
        else:
            print("-> \33[32mNo Errors Were Found \U0001F60A")

    def _timeout(self):
        print('Timeout invoked.')
        self.msgId +=1
        if self.heloSent:
            self.message = build_heartbeat(self.msgId)
            self.heloSent = False
            self.hrbtSent = True
        elif self.hrbtSent:
            self.message = build_pairData(self.msgId)
            self.hrbtSent = False
            self.pdrqSent = True
        elif self.pdrqSent:
            self.message = build_newOrder(self.msgId)
            self.pdrqSent = False
            self.ornwSent = True
        elif self.ornwSent:
            self.message = build_openOrders(self.msgId)
            self.ornwSent = False
            self.oprqSent = True
        elif self.oprqSent:
            self.message = build_balanceReq(self.msgId)
            self.oprqSent = False
            self.blrqSent = True
        elif self.blrqSent:
            self.message = build_cancel(self.msgId)
            self.blrqSent = False
            self.orcnSent = True
        elif self.allSent:
            self.transport.close()
        else:
            self.message = build_heartbeat(self.msgId)
            self.allSent = True
        self._send_msg()
        # setup timeout
        self.timeout_handle = self.loop.call_later(
            TIMEOUT, self._timeout,
        )

    def _send_msg(self):
        self.transport.write(self.message.encode())
        print('Data sent: {!r}'.format(self.message))


async def main():
    # Get a reference to the event loop as we plan to use
    # low-level APIs.
    loop = asyncio.get_running_loop()

    on_con_lost = loop.create_future()
    # message = 'Hello World!'

    print ('connecting client to port 6000')
    transport, protocol = await loop.create_connection(
        lambda: EchoClientProtocol(on_con_lost),
        '127.0.0.1', 6000)

    # Wait until the protocol signals that the connection
    # is lost and close the transport.
    try:
        await on_con_lost
    finally:
        transport.close()

asyncio.run(main())

