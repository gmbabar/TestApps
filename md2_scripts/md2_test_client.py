#!/usr/bin/python3
import asyncio, json, websockets, argparse

class Md2ProtocolClient:
    msg = {}
    symbols = []
    Errors = []
    Warnings = []
    Levels = ["L1", "L2", "T", "S"]
    sides = [1, -1]
    symbolIdx = 1
    LevelIdx = 0
    counter = 0
    unsubscribed = False
    def __init__(self, host, port):
        try:
            self.websocket_ = websockets.connect(f'ws://{host}:{port}')
        except:
            print(f"Unable To Connect With \'{host}:{port}\'")
        self.msg = self.build_secList()
    
    def build_subscribe(self, symbol, level):
        if level != "S":
            return json.dumps({"type":"subscribe","exchange":"DERI","symbol":symbol,"level":level})
        else:
            return json.dumps({"type":"subscribe","exchange":"DERI","symbol":symbol,"level":level,"update_frequency":2,"time_window_secs":5,"max_levels":5})
    
    def build_secList(self):
        return json.dumps({"type" : "security_list_request"})

    def build_unsubscribe(self, symbol, level):
        return json.dumps({"type":"unsubscribe","exchange":"DERI","symbol":symbol,"level":level})
    # {'type': 'security_list', 'symbols': ['DERI:UNI/USDC_PS/UNI', 'DERI:SOL/USD_PS/SOL', 'DERI:BTC/USD_PS/BTC', 'DERI:ETH/USD_PS/ETH']}
    def parseSecList(self, json):
        if 'symbols' not in json:
            self.Errors.append("No Symbols Received In SecurityList")
        else:
            symbols = json['symbols']
            for symbol in symbols:
                if symbol.find("DERI:") != -1:
                    symbol = symbol.replace("DERI:", "")
                self.symbols.append(symbol)
            # print(self.symbols[2])
    
    # {'type': 'inside', 'key': [0, 0], 'symbol': 'SOL/USD_PS/SOL', 'bid': 31.61, 'bidqty': 93320, 'offer': 31.67, 'offerqty': 72750, 'exchange': 'DERI', 'isIntra': False} 
    def parseL1(self, jsonMsg):
        symbol = jsonMsg['symbol']
        msgType = jsonMsg['type'].upper()
        errorMsg = ""
        if symbol != self.symbols[self.symbolIdx]:
            errorMsg = f"{msgType}: Wrong Symbol \'{symbol}\' Received while Sent Request For \'{self.symbols[self.symbolIdx]}\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)

        exchange = jsonMsg['exchange'].upper()
        if exchange != "DERI":
            errorMsg = f"{msgType}: Invalid Exchange \'{exchange}\' Received while Request Sent In \'DERI\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)

        bid = jsonMsg['bid']
        if bid <= 0:
            errorMsg = f"{msgType}: Invalid price \'{bid}\' For Bid Received, Symbol = {self.symbols[self.symbolIdx]}"
            if errorMsg not in self.Warnings:
                self.Warnings.append(errorMsg)

        offer = jsonMsg['offer']
        if offer <= 0:
            errorMsg = f"{msgType}: Invalid price \'{offer}\' For Offer Received, Symbol = {self.symbols[self.symbolIdx]}"
            if errorMsg not in self.Warnings:
                self.Warnings.append(errorMsg)

        # bidqty = jsonMsg['bidqty']
        # if bidqty <= 0:
        #     errorMsg = f"{msgType}: Invalid qty \'{bidqty}\' For Bid Received, Symbol = {self.symbols[self.symbolIdx]}"
        #     if errorMsg not in self.Warnings:
        #         self.Warnings.append(errorMsg)

        # offerqty = jsonMsg['offerqty']
        # if offerqty <= 0:
        #     errorMsg = f"{msgType}: Invalid qty \'{offerqty}\' For Bid Received, Symbol = {self.symbols[self.symbolIdx]}"
        #     if errorMsg not in self.Warnings:
        #         self.Warnings.append(errorMsg)

    
    # {"type":"L2","key":[0,0],"symbol":"BTC/USD_PS/BTC","price":18701.5,"qty":41970,"exchange":"DERI","qlen":1,"side":-1,"isIntra":false}
    def parseL2(self, jsonMsg):
        symbol = jsonMsg['symbol']
        msgType = jsonMsg['type'].upper()
        errorMsg = ""
        if symbol != self.symbols[self.symbolIdx]:
            errorMsg = f"{msgType}: Wrong Symbol \'{symbol}\' Received while Sent Request For \'{self.symbols[self.symbolIdx]}\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)
        
        exchange = jsonMsg['exchange'].upper()
        if exchange != "DERI":
            errorMsg = f"{msgType}: Invalid Exchange \'{exchange}\' Received while Request Sent In \'DERI\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)
        
        price = jsonMsg['price']
        if price <= 0:
            errorMsg = f"{msgType}: Invalid Price \'{price}\' Received In Message, Symbol = {self.symbols[self.symbolIdx]}"
            if errorMsg not in self.Warnings:
                self.Warnings.append(errorMsg)
        
        # qty = jsonMsg['qty']
        # if qty <= 0:
        #     errorMsg = f"{msgType}: Invalid Quantity \'{qty}\' Received In Message, Symbol = {self.symbols[self.symbolIdx]}"
        #     if errorMsg not in self.Warnings:
        #         self.Warnings.append(errorMsg)
        
        side = jsonMsg['side']
        if side not in self.sides:
            errorMsg = f"{msgType}: Invalid Side \'{side}\' Received In Message, Symbol = {self.symbols[self.symbolIdx]}"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)
        


    # {"type":"trade","key":[0,0],"symbol":"BTC/USD_PS/BTC","exchange":"DERI","price":18696.5,"qty":12080,"isIntra":false}
    def parseTrade(self, jsonMsg):
        symbol = jsonMsg['symbol']
        msgType = jsonMsg['type'].upper()
        errorMsg = ""
        if symbol != self.symbols[self.symbolIdx]:
            errorMsg = f"{msgType}: Wrong Symbol \'{symbol}\' Received while Sent Request For \'{self.symbols[self.symbolIdx]}\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)
        
        exchange = jsonMsg['exchange'].upper()
        if exchange != "DERI":
            errorMsg = f"{msgType}: Invalid Exchange \'{exchange}\' Received while Request Sent In \'DERI\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)

        price = jsonMsg['price']
        if price <= 0:
            errorMsg = f"{msgType}: Invalid Price \'{price}\' Received In Msg, Symbol = {self.symbols[self.symbolIdx]}"
            if errorMsg not in self.Warnings:
                self.Errors.Warnings(errorMsg)
        
        # qty = jsonMsg['qty']
        # if qty <= 0:
        #     errorMsg = f"{msgType}: Invalid Quantity \'{qty}\' Received In Message, Symbol = {self.symbols[self.symbolIdx]}"
        #     if errorMsg not in self.Warnings:
        #         self.Warnings.append(errorMsg)
    
    # {"type":"snapshot","globalSeq":1,"inside":"{"bid":18928,"bidqty":2500,"offer":18928.5,"offerqty":2500,"exchange":"DERI"}","isIntra":false,"key":[0,0],"live":1,"localSeq":0,"microsSincEpoch":1665676658805454,"symbol":"BTC/USD_PS/BTC","levels":[{"price":18928,"qty":2500,"qlen":1,"side":1},{"price":18925.5,"qty":4200,"qlen":1,"side":1},{"price":18911,"qty":50000,"qlen":1,"side":1},{"price":18891.5,"qty":1000,"qlen":1,"side":1},{"price":18865,"qty":20000,"qlen":1,"side":1},{"price":18928.5,"qty":2500,"qlen":1,"side":-1},{"price":18932.5,"qty":740,"qlen":1,"side":-1},{"price":18933,"qty":8990,"qlen":1,"side":-1},{"price":18934,"qty":2500,"qlen":1,"side":-1},{"price":18934.5,"qty":2000,"qlen":1,"side":-1}]}
    def parseSnapshot(self, jsonMsg):
        msgType = jsonMsg['type'].upper()
        errorMsg = ""
        symbol = jsonMsg['symbol']
        if symbol != self.symbols[self.symbolIdx]:
            errorMsg = f"{msgType}: Wrong Symbol \'{symbol}\' Received while Sent Request For \'{self.symbols[self.symbolIdx]}\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)
        inside = json.loads(jsonMsg['inside'])
        insideBid = inside['bid']
        if insideBid <= 0:
            errorMsg = f"{msgType}: Invalid Bid Price \'{insideBid}\' Received In \'INSIDE\'"
            if errorMsg not in self.Warnings:
                self.Warnings.append(errorMsg)

        insideOffer = inside['offer']
        if insideOffer <= 0:
            errorMsg = f"{msgType}: Invalid Offer Price \'{insideOffer}\' Received In \'INSIDE\'"
            if errorMsg not in self.Warnings:
                self.Warnings.append(errorMsg)
        exchange = inside['exchange'].upper()
        if exchange != "DERI":
            errorMsg = f"{msgType}: Invalid Exchange \'{exchange}\' Received while Request Sent In \'DERI\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)
        # {"price":18928,"qty":2500,"qlen":1,"side":1}
        levels = jsonMsg['levels']
        price = 0
        side = 0
        for data in levels:
            if 'price' in data:
                price = data['price']
                if price <= 0:
                    errorMsg = f"{msgType}: Invalid Price \'{price}\' Received In \'Levels\'"
                    if errorMsg not in self.Warnings:
                        self.Warnings.append(errorMsg)
            else:
                errorMsg = f"{msgType}: Price Not Received Inside \'Levels\'"
                if errorMsg not in self.Warnings:
                    self.Warnings.append(errorMsg)
            if 'side' in data:
                side = data['side']
                if side not in self.sides:
                    errorMsg = f"{msgType}: Invalid Side \'{side}\' Received In \'Levels\'"
                    if errorMsg not in self.Warnings:
                        self.Warnings.append(errorMsg)
            else:
                errorMsg = f"{msgType}: Side Not Received Inside \'Levels\'"
                if errorMsg not in self.Errors:
                    self.Errors.append(errorMsg)


    # {"type":"subscribe","exchange":"DERI","symbol":"BTC/USD_PS/BTC","level":"L2"}
    def parseSubscribed(self, jsonMsg):
        msgType = jsonMsg['type'].upper()

        symbol = jsonMsg['symbol']
        errorMsg = ""
        if symbol != self.symbols[self.symbolIdx]:
            errorMsg = f"{msgType}: Wrong Symbol \'{symbol}\' Received while Request Sent For \'{self.symbols[self.symbolIdx]}\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)
        
        exchange = jsonMsg['exchange'].upper()
        if exchange != "DERI":
            errorMsg = f"{msgType}: Invalid Exchange \'{exchange}\' Received while Request Sent In \'DERI\'"
            if errorMsg not in self.Errors:
                self.Errors.append(errorMsg)
        
        level = jsonMsg['level']
        currLevel = self.Levels[self.LevelIdx]
        if level != currLevel:
            errorMsg = f"{msgType}: Invalid Level Subscribed \'{level}\' while Request Sent For \'{currLevel}\'"


    def printErrors(self):
        if len(self.Errors) == 0 and len(self.Warnings) == 0:
            print("-> \33[32mNo Errors Were Found \U0001F44C")
        elif len(self.Errors) == 0 and len(self.Warnings) != 0:
            print("-> \33[32mNo Errors Were Found \U0001F44C")
            print(f"-> \33[33mFound {len(self.Warnings)} Warnings \U000026A0")
            for warning in self.Warnings:
                print(f"\t\33[33m{warning}")
        else:
            print(f"-> \33[31mFound {len(self.Errors)} Errors \U0001F62Bs")
            for error in self.Errors:
                print(f"\t\33[31m{error}")
            print(f"-> \33[33mFound {len(self.Warnings)} Warnings \U000026A0")
            for warning in self.Warnings:
                print(f"\t\33[33m{warning}")

    async def _recv_msg(self, websocket_):
            errorMsg = ""
            response = await websocket_.recv()
            print(f"Received: {response}")
            try:
                # print(f"Received: {json.loads(response)}")
                json.loads(response)
            except:
                print(f"-> \33[36mInvalid Json Received\nsent: {self.msg}")
                self.printErrors()
                await websocket_.close()
                exit()
            if 'type' not in json.loads(response):
                self.Errors.append("No Message Type Received In Response")
                return
            msgType = json.loads(response)['type']
            if msgType == 'security_list':
                self.parseSecList(json.loads(response))
                self.msg = self.build_subscribe(self.symbols[self.symbolIdx], self.Levels[self.LevelIdx])
                await self._send_msg()
            
            if msgType == 'subscribed':
                self.parseSubscribed(json.loads(response))
            if msgType != 'security_list':
                if msgType != 'subscribed':
                    errorMsg = f"No Confirmation Response Message Received MSGTYPE=\'{msgType}\'"
                    if errorMsg not in self.Errors:
                        self.Errors.append(errorMsg)
            while websocket_.open:
                response = await websocket_.recv()
                response = json.loads(response)
                print(f"Received {self.counter+1}: {response}")
                msgType = response['type']
                if msgType == "inside":
                    self.parseL1(response)
                elif msgType == "L2":
                    self.parseL2(response)
                elif msgType == "T":
                    self.parseTrade(response)
                elif msgType == "snapshot":
                    self.parseSnapshot(response)
                elif msgType == "subscribed":
                    errorMsg = f"Received Confirmation After The Message"
                    self.Warnings.append(errorMsg)
                self.counter +=1
                if self.counter%10==0:
                    self.msg = self.build_unsubscribe(self.symbols[self.symbolIdx], self.Levels[self.LevelIdx])
                    if self.LevelIdx == 2 and self.symbolIdx == 2: #For Trade Msg Use BTC Symbol Only
                        self.symbolIdx +=1
                    self.symbolIdx +=1
                    self.unsubscribed = True
                    await self._send_msg()

        
    async def _send_msg(self):
        async with self.websocket_ as websocket:
            await websocket.send(self.msg.encode())
            print(f'Sent: {self.msg}')
            if self.unsubscribed:
                    if self.symbolIdx >= len(self.symbols):
                        print("--> Changing The Data Type")
                        self.symbolIdx = 1
                        self.LevelIdx +=1
                        if self.LevelIdx == 2:
                            self.symbolIdx = 2
                        if self.LevelIdx == 3:
                            self.symbolIdx = 2
                        if self.LevelIdx >= len(self.Levels):
                            
                            self.printErrors()
                            await websocket.close()

                            exit()
                        # await websocket.close()
                        # exit()
                    self.msg = self.build_subscribe(self.symbols[self.symbolIdx], self.Levels[self.LevelIdx])
                    self.unsubscribed = False
                    await self._send_msg()
            await self._recv_msg(websocket)
    

def init_argparse() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description='Test client for MA2 deribit implementation')
    # '-h' is reserved for help by argparse package, so '-s' is forced choice for host/server
    parser.add_argument('-s', '--host', help='server (host) name or ip address for MA2 server', required=True)
    parser.add_argument('-p', '--port', help='port number for MA2 server', required=True, type=int)
    # args = vars(parser.parse_args())
    return parser


if __name__=="__main__":
    # cmd line args parser
    parser = init_argparse()
    args = parser.parse_args()
    host = args.host
    port = args.port

    client = Md2ProtocolClient(host, port)
    asyncio.get_event_loop().run_until_complete(client._send_msg())