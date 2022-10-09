#!/usr/bin/python3

'''
https://docs.python.org/3/library/asyncio-protocol.html
'''

import asyncio

TIMEOUT = 10 #seconds.

class EchoServerProtocol(asyncio.Protocol):
    def connection_made(self, transport):
        peername = transport.get_extra_info('peername')
        print('Connection from {}'.format(peername))
        self.transport = transport
        loop = asyncio.get_running_loop()
        self.timeout_handle = loop.call_later(
            TIMEOUT, self._timeout,
        )

    def data_received(self, data):
        message = data.decode()
        print('Data received: {!r}'.format(message))

        print('Send: {!r}'.format(message))
        self.transport.write(data)

        #print('Close the client socket')
        #self.transport.close()

    def _timeout(self):
        print('Timeout invoked.')
        self.transport.close()


async def main():
    # Get a reference to the event loop as we plan to use
    # low-level APIs.
    loop = asyncio.get_running_loop()

    print ('creating socket on port 8888')
    server = await loop.create_server(
        lambda: EchoServerProtocol(),
        '127.0.0.1', 8888)

    async with server:
        print ('waiting client on port 8888')
        await server.serve_forever()


asyncio.run(main())

