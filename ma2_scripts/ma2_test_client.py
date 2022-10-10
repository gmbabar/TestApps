#!/usr/bin/python3

'''
https://docs.python.org/3/library/asyncio-protocol.html
'''

def build_heartbeat(msgId):
    return '{ "id": ' + str(msgId) + ', "type":"hrbt" }'

import asyncio

TIMEOUT = 1 #seconds.

class EchoClientProtocol(asyncio.Protocol):
    msgId = 0;
    def __init__(self, message, on_con_lost):
        self.message = message
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
        print('Data received: {!r}'.format(data.decode()))

    def connection_lost(self, exc):
        print('The server closed the connection')
        self.on_con_lost.set_result(True)

    def _timeout(self):
        print('Timeout invoked.')
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
    message = 'Hello World!'

    print ('connecting client to port 8888')
    transport, protocol = await loop.create_connection(
        lambda: EchoClientProtocol(message, on_con_lost),
        '127.0.0.1', 8888)

    # Wait until the protocol signals that the connection
    # is lost and close the transport.
    try:
        await on_con_lost
    finally:
        transport.close()

asyncio.run(main())

