#!/usr/bin/python3
import psutil
import subprocess


def bina_process_check(process_name:str):
    futures = False
    spot = False
    margin = False
    for process in psutil.process_iter():
        if process.name() == process_name:
            if "md2_futures_pf.json" in " ".join(process.cmdline()):
                futures = True
            elif "md2_swaps_pf.json" in " ".join(process.cmdline()):
                margin = True
            elif "md2_spot_pf.json" in " ".join(process.cmdline()):
                spot = True

    if not futures:
        subprocess.Popen(["/usr/bin/nohup", "/home/ubuntu/GBabar/Binance/run_binancemd2_fut.sh"])
    if not spot:
        subprocess.Popen(["/usr/bin/nohup", "/home/ubuntu/GBabar/Binance/run_binancemd2_spot.sh"])
    if not margin:
        subprocess.Popen(["/usr/bin/nohup", "/home/ubuntu/GBabar/Binance/run_binancemd2_swaps.sh"])

    if futures and spot and margin:
        print("Every Instance Running")

if __name__ == "__main__":
    bina_process_check("binancespotpfmd2_d")