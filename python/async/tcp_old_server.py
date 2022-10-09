#!/usr/bin/python3

import asyncio, socket

async def handle_client(client):
    loop = asyncio.get_event_loop()
    request = None
    while request != 'quit':
        request = (await loop.sock_recv(client, 255)).decode('utf8')
        response = str(request) + '\n'
        await loop.sock_sendall(client, response.encode('utf8'))
    print('closing client socket')
    client.close()

async def run_server():
    print ('creating socket...')
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print ('binding socket to port 23456...')
    server.bind(('localhost', 23456))
    server.listen(8)
    print ('setting socket non-blocking...')
    server.setblocking(False)

    loop = asyncio.get_event_loop()

    while True:
        print ('waiting for clients on socket 23456...')
        client, _ = await loop.sock_accept(server)
        loop.create_task(handle_client(client))

if __name__ == '__main__':
    asyncio.run(run_server())


