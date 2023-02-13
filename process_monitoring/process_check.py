#!/usr/bin/python3
import psutil
import subprocess


def bina_process_check(process_name:str):
    # pids = []
    commands = {}
    futures = False
    spot = False
    margin = False
    for process in psutil.process_iter():
        if process.name() == process_name and process.pid not in commands:
            print (" ".join(process.cmdline()))
            if "md2_futures_pf.json" in " ".join(process.cmdline()):
                futures = True
            elif "md2_swaps_pf.json" in " ".join(process.cmdline()):
                margin = True
            elif "md2_spot_pf.json" in " ".join(process.cmdline()):
                spot = True

    if not futures:
        print("Starting FuturesPF")
        subprocess.Popen(["/usr/bin/nohup", "/home/users/gbabar/work/BinanceGw/run_md2_pf_fut.sh"])
    if not spot:
        print("Starting SpotPF")
        subprocess.Popen(["/usr/bin/nohup", "/home/users/gbabar/work/BinanceGw/run_md2_pf.sh"])
    if not margin:
        print("Starting MarginPF")
        subprocess.Popen(["/usr/bin/nohup", "/home/users/gbabar/work/BinanceGw/run_md2_pf_swap.sh"])

    if futures and spot and margin:
        print("Every Process Running")

if __name__ == "__main__":
    bina_process_check("binancespotpfmd2_d")