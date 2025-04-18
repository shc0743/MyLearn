import ctypes
import random
import threading
from ipaddr import get_ethernet_ipv6, is_valid_ipv6
from time import sleep

import win32serviceutil
import win32service
import win32event
import servicemanager
import socket
import sys, os
import subprocess
import uuid
import datetime

from events import write_to_event_log
from eventprovider import load
def log(message, event_id, event_type=servicemanager.EVENTLOG_INFORMATION_TYPE):
    write_to_event_log("Application", "update-ip", event_id, event_type, message)

def regEvent(msgFile):
    content = r'''Windows Registry Editor Version 5.00

[HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\EventLog\Application\update-ip]
"EventMessageFile"="''' + msgFile + '''"
"TypesSupported"=dword:00000007
"CategoryMessageFile"="''' + msgFile + '''"
"CategoryCount"=dword:00000001
'''
    try:
        file = uuid.uuid4().hex + ".reg"
        with open(file, "w") as f:
            f.write(content)
        # 执行注册表文件
        subprocess.run(["reg", "import", file], check=True)
        # 删除临时文件
        os.remove(file)
    except Exception as e:
        print(f"Error registering event source: {e}")

def unregEvent():
    # 删除注册表项
    try:
        import winreg
        key = winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"SYSTEM\CurrentControlSet\Services\EventLog\Application", 0, winreg.KEY_ALL_ACCESS)
        winreg.DeleteKey(key, "update-ip")
        winreg.CloseKey(key)
    except Exception as e:
        print(f"Error deleting registry key: {e}")

def update_logic():
    # 这里是更新逻辑的实现
    # 例如，调用 update_cloudflare 函数来更新 DNS 记录
    log('', 0x17, servicemanager.EVENTLOG_INFORMATION_TYPE)
    # 打开一个子进程，记录输出到 变量
    command = ['python', __file__ + '/../update-ip.py']
    try:
        n = 0
        while n < 3:
            result = subprocess.run(command, shell=False, text=True, capture_output=True)
            log(f"{result.stdout}", 1000, servicemanager.EVENTLOG_INFORMATION_TYPE)
            if result.returncode != 0:
                n = n + 1
                log(f"[Exit code={result.returncode}] Error: {result.stderr}{f'\nWe will retry after {n * 15} seconds...' if (n < 3) else ''}", 0x20, servicemanager.EVENTLOG_WARNING_TYPE if (n < 3) else servicemanager.EVENTLOG_ERROR_TYPE)
                sleep(n * 15)
            else:
                log('', 0x18, servicemanager.EVENTLOG_INFORMATION_TYPE)
                break
    except Exception as e:
        log(f"Error executing subprocess: {e}", 0x3B, servicemanager.EVENTLOG_ERROR_TYPE)
        log('', 0x19, servicemanager.EVENTLOG_ERROR_TYPE)

class SimpleService(win32serviceutil.ServiceFramework):
    _svc_name_ = "update-ip_Service"
    _svc_display_name_ = "update-ip Service"
    _svc_description_ = "The service to update ip"

    def __init__(self, args):
        win32serviceutil.ServiceFramework.__init__(self, args)
        self.hWaitStop = win32event.CreateEvent(None, 0, 0, None)
        socket.setdefaulttimeout(60)
        self.is_alive = True

    def SvcStop(self):
        self.is_alive = False
        self.ReportServiceStatus(win32service.SERVICE_STOP_PENDING)
        log('', 0x38, servicemanager.EVENTLOG_INFORMATION_TYPE)
        win32event.SetEvent(self.hWaitStop)

    def SvcDoRun(self):
        servicemanager.LogMsg(
            servicemanager.EVENTLOG_INFORMATION_TYPE,
            servicemanager.PYS_SERVICE_STARTED,
            (self._svc_name_, '')
        )
        log('', 0x33, servicemanager.EVENTLOG_INFORMATION_TYPE)
        sleep(10) # 等待10秒，确保服务启动完成
        self.main()

    @classmethod
    def installPrep(cls):
        # 先注册事件源
        file_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'UpdateIpResource.dll').replace('\\', '/')
        load(file_name)
        regEvent(file_name)

    @classmethod
    def removePrep(cls):
        # 注销事件源
        unregEvent()
        file_name = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'UpdateIpResource.dll').replace('\\', '/')
        ctypes.windll.kernel32.MoveFileExW.argtypes = [ctypes.c_wchar_p, ctypes.c_wchar_p, ctypes.c_uint]
        ctypes.windll.kernel32.MoveFileExW.restype = ctypes.c_bool
        # 先尝试普通删除
        try:
            os.remove(file_name)
            print(f"File cleanup success.")
        except BaseException:
            try:
                if ctypes.windll.kernel32.MoveFileExW(file_name, None, 4):
                    print('File deletion scheduled. Restart your computer to complete the deletion.')
                else:
                    print('Failed to MoveFileExW. Please check your permissions.')
            except BaseException:
                print("Unable to delete the file. Please delete it manually.")

    def mysleep(self, seconds):
        """可中断的睡眠函数，每隔1秒检查服务是否应停止"""
        for _ in range(seconds):
            if not self.is_alive:  # 检查停止标志
                return
            sleep(1)

    def main(self):
        previous_ip = ''
        n = 0
        while self.is_alive:
            try:
                current_ip = get_ethernet_ipv6()
                if current_ip and is_valid_ipv6(current_ip):
                    if current_ip != previous_ip:
                        log(current_ip, 0x14, servicemanager.EVENTLOG_INFORMATION_TYPE)
                        log([previous_ip, current_ip], 0x15, servicemanager.EVENTLOG_INFORMATION_TYPE)
                        if current_ip and current_ip != '' and current_ip != None:
                            # 记录日志。0x16,  		"update-ip 任务已规划。预计执行时间：%1\r\n"
                            log(str(datetime.datetime.now() + datetime.timedelta(seconds=3)), 0x16, servicemanager.EVENTLOG_INFORMATION_TYPE)
                            # 等待3秒
                            self.mysleep(3)
                            # 如果再次变化，则进入下一轮
                            current_ip_2 = get_ethernet_ipv6()
                            if not current_ip == current_ip_2:
                                previous_ip = current_ip
                                continue
                            # 在新的线程中执行更新操作
                            thread = threading.Thread(target=update_logic, daemon=True)
                            thread.start()
                            thread.join()
                            # 等待更新完成
                            while thread.is_alive():
                                sleep(1)
                                if not self.is_alive:
                                    return
                        # 完成更新
                        previous_ip = current_ip
                self.mysleep(10)
                if not self.is_alive:
                    break
                if n % (360 * 2) == 0:  # 每2小时显示一次广告（假设每次循环耗时10秒，360次循环为1小时）
                    # 随机显示广告
                    ads = [1013, 1218, 1998, 1999, 2000]
                    random_event_id = random.choice(ads)
                    log("", random_event_id, servicemanager.EVENTLOG_INFORMATION_TYPE)
                n += 1 # 确保第一次启动时可以显示广告
            except BaseException as e:
                log(f"Error in main loop: {e}", 0x3B, servicemanager.EVENTLOG_ERROR_TYPE)
                sleep(60)

if __name__ == '__main__':
    if len(sys.argv) == 1:
        servicemanager.Initialize()
        servicemanager.PrepareToHostSingle(SimpleService)
        servicemanager.StartServiceCtrlDispatcher()
    else:
        if sys.argv[1] == 'install':
            SimpleService.installPrep()
            subprocess.Popen('timeout 3 & sc config "update-ip_Service" start= delayed-auto depend=Tcpip/Http/Ntfs/DcomLaunch', shell=True, creationflags=subprocess.CREATE_NO_WINDOW)
        elif sys.argv[1] == 'remove':
            SimpleService.removePrep()
        win32serviceutil.HandleCommandLine(SimpleService)
