#!/usr/bin/python3

import psutil
from datetime import datetime
import os
import signal
import subprocess
import argparse
import time


def get_process_info(process_name: str):
    found = False
    pids = []
    commands = {}
    for process in psutil.process_iter():
        if process.name() != process_name:
            continue
        try:
            createTime = datetime.fromtimestamp(process.create_time())
        except OSError:
            createTime = datetime.fromtimestamp(psutil.boot_time())
        try:
            username = process.username()
        except psutil.AccessDenied:
            username = 'N/A'
        threadsNo = process.num_threads()
        try:
            cpuUsage = process.cpu_percent(1)
        except KeyboardInterrupt:
            print("Script Stopped By User")
            exit()
        except:
            print("Failed To Get CPU Usage")
            return
        try:
            status = process.status()
        except:
            print("Failed To Get Process Status")
            return
        try:
            memoryUsage = process.memory_percent('uss')
        except:
            print("Failed To Get Memory Usage For Process")
            return
        try:
            ioCounters = process.io_counters()
        except:
            print("Failed to GET I/O Counters For Process")
            return
        try:
            readBytes = ioCounters.read_bytes
        except:
            print("Failed To Get Read Bytes For Process")
            return
        try:
            writeBytes = ioCounters.write_bytes
        except:
            print("Failed To Get Write Bytes For Process")
            return
        print({
            "process_name": process_name,
            "pid": process.pid,
            "create_time": createTime,
            "started_by": username,
            "no_of_threads_spawned": threadsNo,
            "cpu_usage": cpuUsage,
            "memory_usage": memoryUsage,
            "status": status,
            "read_bytes": readBytes,
            "write_bytes": writeBytes
        })
        if cpuUsage > 10.0 or memoryUsage > 5.0:
            cmd = process.cmdline()
            os.kill(process.pid, signal.SIGTERM)
            time.sleep(1)
            subprocess.Popen(["/usr/bin/nohup"] + cmd)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Process monitor and relauncher in case of excessive resource pegging')
    # '-h' is reserved for help by argparse package, so '-s' is forced choice for host/server
    parser.add_argument('-p', "--process", help="Process Name To Monitor", required=True)
    args = parser.parse_args()
    process = args.process
    get_process_info(process)
