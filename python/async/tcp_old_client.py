#!/usr/bin/python3

import asyncio, socket

async def handle_client(client):
    loop = asyncio.get_event_loop()
    msg = "hello from client"
    print (f"sending msg: {msg}")
    await loop.sock_sendall(client, msg.encode('utf8'))
    print (f"waiting for response")
    request = (await loop.sock_recv(client, 255)).decode('utf8')
    print (f"response received: {request}")
    msg = "quit"
    await loop.sock_sendall(client, msg.encode('utf8'))
    client.close()

async def run_server():
    print ('creating socket...')
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    print ('connecting to port 23456...')
    client.connect(('localhost', 23456))
    print ('setting socket non-blocking...')
    client.setblocking(False)
    print ('done...')

    loop = asyncio.get_event_loop()

    print ("creating task: handle_client")
    #loop.create_task(handle_client(client))
    await asyncio.gather(handle_client(client))

if __name__ == '__main__':
    asyncio.run(run_server())


