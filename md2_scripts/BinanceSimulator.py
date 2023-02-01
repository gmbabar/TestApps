import websocket
import json

def on_message(ws, message):
    print("Received message: " + message)

def on_error(ws, error):
    print("Received error: " + error)

def on_close(ws):
    print("Connection closed")

def on_open(ws):
    message = {"message": "Hello, Server!"}
    ws.send(json.dumps(message))
    print("Sent message: " + json.dumps(message))

if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("wss://echo.websocket.org/",
                                on_message=on_message,
                                on_error=on_error,
                                on_close=on_close)
    ws.on_open = on_open
    ws.run_forever()
