import asyncio
import websockets
import json

async def send_message(websocket):
    message = {"message": "Hello, Server!"}
    await websocket.send(json.dumps(message))
    print("Sent message: " + json.dumps(message))

async def receive_message(websocket):
    message = await websocket.recv()
    print("Received message: " + message)

async def main():
    async with websockets.connect("ws://localhost:9443") as websocket:
        await send_message(websocket)
        await receive_message(websocket)

if __name__ == "__main__":
    asyncio.get_event_loop().run_until_complete(main())
