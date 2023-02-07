import psutil
from datetime import datetime
import os
import signal
import subprocess
import argparse


def get_process_info(process_name:str, alternateCommands:list):
    found = False
    pids = []
    commands = {}
    for process in psutil.process_iter():
        if process.name() == process_name and process.pid not in pids:
            pids.append(process.pid)
            commandLine = process.cmdline()
            actual = str()
            for command in commandLine:
                actual+=str(command + ' ')
            actual = actual.rstrip(actual[-1])
            commands[process.pid] = actual

    if len(pids) <= 0:
        for command in alternateCommands:
            subprocess.Popen(["nohup", command])
        return

    for pid in pids:
        try:
            process = psutil.Process(pid)
        except psutil.NoSuchProcess:
            subprocess.Popen(["nohup", commands[pid]])
            return
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
            "process_name":process_name,
            "pid":pid,
            "create_time":createTime,
            "started_by":username,
            "no_of_threads_spawned":threadsNo,
            "cpu_usage":cpuUsage,
            "memory_usage":memoryUsage,
            "status":status,
            "read_bytes":readBytes,
            "write_bytes":writeBytes
        })
        if cpuUsage > 1.5 or memoryUsage > 2.0:
            os.kill(pid, signal.SIGTERM)
            subprocess.Popen(["nohup", commands[pid]])


def init_argparse() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description='Test client for MA2 deribit implementation')
    # '-h' is reserved for help by argparse package, so '-s' is forced choice for host/server
    parser.add_argument('-p', "--process", help="Process Name To Monitor", required=True)
    parser.add_argument('-e', '--executables', help='Executable List For Binaries', required=True)
    return parser

if __name__ == "__main__":
    parser = init_argparse()
    args = parser.parse_args()
    process = args.process
    executables = args.executables
    print(list(executables.split(',')))
    get_process_info(process, executables)