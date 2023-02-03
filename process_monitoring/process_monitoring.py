import psutil
from datetime import datetime
import os
import subprocess

def get_process_info(process_name:str):
    found = False
    for process in psutil.process_iter():
        if process.name() == process_name:
            found = True
            pid = process.pid
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
            return {
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
            }
    if not found:
        try:
            print("No Process Found")
            return
        except KeyboardInterrupt:
            print("Script Stopped By User")
            exit()


if __name__ == "__main__":
    logs = []
    process_name = "PerformanceTest"
    process_executable = "/home/harismirza/Desktop/Coding/Clones/TestApps/TESTING FOLDER/PerformanceTest"
    while True:
        data = get_process_info(process_name)
        if data != None:
            # print(data)
            # print(type(data['cpu_usage']))
            if data["cpu_usage"] > 7.5 or data['memory_usage'] > 2.0:
                pid = data['pid']
                os.system(f"kill {pid}")
                print("----------------->>>> Killed The Process <<<<-----------------")
            logs.append(data)
        else:
            subprocess.Popen(["nohup", process_executable])