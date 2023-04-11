#!/usr/bin/python3

import time
import random
import os
import sys

def readData(filepath: str):
    orderDetail = {}
    btctrades = []
    ethtrades = []
    with open(filepath, "r+") as file:
        data = file.readlines()
        for line in data:
            if line.find('trade') == -1:
                continue
            if line.find('BTC-PERP') != -1:
                btctrades.append(line)
            if line.find('ETH-PERP') != -1:
                ethtrades.append(line)
        # preserve last 4-5 trades only. we don't care about older data.
        file.seek(0)
        for x in range(5):
            try:
                file.write(btctrades[x-5])
            except:
                pass    # duck exception
            try:
                file.write(ethtrades[x-5])
            except:
                pass    # duck exception
        file.truncate()
        if len(btctrades) != 0:
            lastTrade = btctrades[-1].split(',')
            price = lastTrade[-3].split(":")[-1].strip()
            symbol = lastTrade[1].split(":")[-1].strip()
            orderDetail[symbol] = float(price)
        if len(ethtrades) != 0:
            lastTrade = ethtrades[-1].split(',')
            price = lastTrade[-3].split(":")[-1].strip()
            symbol = lastTrade[1].split(":")[-1].strip()
            orderDetail[symbol] = float(price)
    file.close()

    return orderDetail

def generateOrderId():
    return random.randint(10000000, 99999999)

def newOrderCmd(symbol: str, side: str, px: float, orderId: int, new: bool=True):
    qty = int((51000.0 / px) * 1e4) * 1e5
    return f"gw_command_d -e localhost:6000 -c \"new_order {symbol} {side} {int(qty)} {px} {orderId}\""

def cancelCmd(orderId:int):
    return f"gw_command_d -e localhost:6000 -c \"cancel_order {orderId}\""

def processTrade(symbol:str, tradePriceNow:float, tradePriceOld:float, buyOrderIds:dict, sellOrderIds:dict):
    tickIncrement = 0.5
    if tradePriceOld == tradePriceNow:
        return
    print (">>> order Info: ", symbol, tradePriceNow)
    buyOrderTop = generateOrderId()
    buyOrderDepth = generateOrderId()
    sellOrderTop = generateOrderId()
    sellOrderDepth = generateOrderId()
    # symbol = orderInfo[1]
    print('New buy order: symbol:',  symbol, ', price:', tradePriceNow - (tickIncrement*1), ', id: ', buyOrderTop)
    print('New buy order: symbol:',  symbol, ', price:', tradePriceNow - (tickIncrement*2), ', id: ', buyOrderDepth)
    print('New sell order: symbol:', symbol, ', price:', tradePriceNow + (tickIncrement*1), ', id: ', sellOrderTop)
    print('New sell order: symbol:', symbol, ', price:', tradePriceNow + (tickIncrement*2), ', id: ', sellOrderDepth)
    gwCommand = newOrderCmd(symbol, "B", tradePriceNow - (tickIncrement*1), buyOrderTop)
    print(gwCommand)
    os.system(gwCommand)
    gwCommand = newOrderCmd(symbol, "B", tradePriceNow - (tickIncrement*2), buyOrderDepth)
    print(gwCommand)
    os.system(gwCommand)
    # print("(---------------------------------)")
    gwCommand = newOrderCmd(symbol, "S", tradePriceNow + (tickIncrement*1), sellOrderTop)
    print(gwCommand)
    os.system(gwCommand)
    gwCommand = newOrderCmd(symbol, "S", tradePriceNow + (tickIncrement*2), sellOrderDepth)
    print(gwCommand)
    os.system(gwCommand)

    if symbol not in buyOrderIds:
        buyOrderIds[symbol] = []
    if symbol not in sellOrderIds:
        sellOrderIds[symbol] = []

    for id in buyOrderIds[symbol]:
        print('Cancelling buy order with id:', id)
        gwCommand = cancelCmd(id)
        print(gwCommand)
        os.system(gwCommand)
    for id in sellOrderIds[symbol]:
        print('Cancelling sell order with id:', id)
        gwCommand = cancelCmd(id)
        print(gwCommand)
        os.system(gwCommand)
    buyOrderIds[symbol].clear()
    sellOrderIds[symbol].clear()
    # append
    buyOrderIds[symbol].append(buyOrderTop)
    buyOrderIds[symbol].append(buyOrderDepth)
    sellOrderIds[symbol].append(sellOrderTop)
    sellOrderIds[symbol].append(sellOrderDepth)
    # tradePriceOld = tradePriceNow

if __name__=="__main__":
    if len(sys.argv) != 2:
        print ('Invalid arguments')
        print ("Usage:")
        print ("\t", sys.argv[0], "<file>")
        sys.exit(0)
    # filePath = "/home/users/gbabar/work/CoinbaseGw/msg.txt"
    filePath = sys.argv[1]
    lastOrderInfo = {}
    buyOrderIds = {}
    sellOrderIds = {}

    while True:
        time.sleep(1)
        orderInfo = readData(filePath)
        for symbol, price in orderInfo.items():
            lastTradePrice = 0
            if symbol in lastOrderInfo:
                lastTradePrice = lastOrderInfo[symbol]
            processTrade(symbol, price, lastTradePrice, buyOrderIds, sellOrderIds)
        lastOrderInfo = orderInfo
