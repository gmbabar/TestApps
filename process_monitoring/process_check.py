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
            if "futures" in " ".join(process.cmdline()):
                futures = True
            elif "margin" in " ".join(process.cmdline()):
                margin = True
            elif "spot" in " ".join(process.cmdline()):
                spot = True

    if not futures:
        print("Running Futures")
        subprocess.Popen(["/usr/bin/nohup", "/home/harismirza/Desktop/Coding/Clones/TestApps/TestingFolder/process.sh", "-futures_pf"])
    if not spot:
        print("Running Spot")
        subprocess.Popen(["/usr/bin/nohup", "/home/harismirza/Desktop/Coding/Clones/TestApps/TestingFolder/process.sh", "-spot_pf"])
    if not margin:
        print("Running Margin")
        subprocess.Popen(["/usr/bin/nohup", "/home/harismirza/Desktop/Coding/Clones/TestApps/TestingFolder/process.sh", "-margin_pf"])

    if futures and spot and margin:
        print("Every Process Running")

if __name__ == "__main__":
    bina_process_check("process.sh")