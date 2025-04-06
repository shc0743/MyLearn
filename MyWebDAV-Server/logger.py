import ctypes
import json
import sqlite3
import re
import win32pipe, win32file, pywintypes

# 创建SQLite表（兼容多日志格式）
conn = sqlite3.connect('access.db')
cursor = conn.cursor()
cursor.execute('''
    CREATE TABLE IF NOT EXISTS logs (
        id INTEGER PRIMARY KEY,
        timestamp TEXT,
        level TEXT DEFAULT 'unknown',
        message TEXT,
        username TEXT,
        address TEXT,
        raw_message TEXT
    )
''')
conn.commit()

# 日志解析正则（覆盖示例中的所有格式）
# LOG_PATTERNS = [
#     # 格式1: 2025-03-23T03:16:14.320+0800	info	listening	{"address": "[::]:40635"}
#     re.compile(r'(?P<timestamp>\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}\+\d{4})\t(?P<level>\w+)\t(?P<message>.+?)\t\{"(?P<json>.+)"\}'),

#     # 格式2: 2025/03/23 11:25:45 http: TLS handshake error...
#     re.compile(r'(?P<timestamp>\d{4}/\d{2}/\d{2} \d{2}:\d{2}:\d{2})\s+(?P<message>.+)'),

#     # 格式3: 混合日志（如多行合并）
#     re.compile(r'(?P<timestamp>\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}\+\d{4})')
# ]
LOG_PATTERNS = [
    # 格式1: 带JSON的日志（如授权、监听事件）
    re.compile(
        r'(?P<timestamp>\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}\.\d{3}\+\d{4})\t'
        r'(?P<level>\w+)\t'
        r'(?P<message>.+)$'  # 匹配到行尾
    ),
    # 格式2: 普通错误日志（如TLS错误）
    re.compile(
        r'(?P<timestamp>\d{4}/\d{2}/\d{2} \d{2}:\d{2}:\d{2})\s+'
        r'(?P<message>.+)'
    )
]

def parse_log(line):
    line = line.strip()
    log_data = {'timestamp': None, 'level': 'unknown', 'message': line, 'username': None, 'remote_address': None}

    log_data['raw'] = line  # 原始日志行

    # 检测并解析JSON格式日志
    if line.startswith('{'):
        try:
            json_data = json.loads(line)
            # 提取字段（适配键名）
            log_data['timestamp'] = json_data.get('ts')
            log_data['level'] = json_data.get('level', 'unknown')
            log_data['message'] = json_data.get('msg', '')
            log_data['username'] = json_data.get('username')
            log_data['remote_address'] = json_data.get('remote_address') or json_data.get('address')
            return log_data
        except json.JSONDecodeError:
            pass  # 解析失败则继续使用正则
    
    for pattern in LOG_PATTERNS:
        match = pattern.search(line)
        if match:
            groups = match.groupdict()
            log_data.update({k:v for k,v in groups.items() if v})
            # 从message中提取level（如有）
            if 'level' not in log_data:
                log_data['level'] = 'error' if 'error' in line.lower() else 'info'
            break
        else:
            # 直接将整行作为message，并带上当前时间
            log_data['timestamp'] = get_current_time()
            log_data['message'] = line
            log_data['level'] = 'unknown'
    
    return log_data

# 命名管道服务（支持单连接）
def pipe_server():
    pipe_name = r'\\.\pipe\WebDAV-Server-Logging-Entrance'
    
    while True:
        try:
            handle = win32pipe.CreateNamedPipe(
                pipe_name,
                win32pipe.PIPE_ACCESS_DUPLEX,
                win32pipe.PIPE_TYPE_MESSAGE | win32pipe.PIPE_READMODE_MESSAGE | win32pipe.PIPE_WAIT,
                1,  # 最大连接数
                65536, 65536, 0, None
            )
            print("Named pipe created. Waiting for connection...", flush=True)
            win32pipe.ConnectNamedPipe(handle, None)
            print("Named pipe: connected", flush=True)
            
            try:
                while True:
                    try:
                        result, data = win32file.ReadFile(handle, 4096)
                        if result == 0 and data:  # 确保读取成功且有数据
                            log_entry = parse_log(data.decode('utf-8'))
                            cursor.execute('''
                                INSERT INTO logs 
                                (timestamp, level, message, username, address, raw_message)
                                VALUES (?, ?, ?, ?, ?, ?)
                            ''', (
                                log_entry['timestamp'],
                                log_entry['level'],
                                log_entry['message'],
                                log_entry['username'],
                                log_entry['remote_address'],
                                log_entry['raw']
                            ))
                            conn.commit()
                    except pywintypes.error as e:
                        if e.winerror in (109, 232):  # 捕获两种断开错误
                            print(f"Client disconnected. Error code: {e.winerror}")
                            break
                        else:
                            raise
            finally:
                try:
                    win32pipe.DisconnectNamedPipe(handle)
                except pywintypes.error as e:
                    if e.winerror != 232:  # 忽略已断开错误
                        raise
                win32file.CloseHandle(handle)
        except KeyboardInterrupt:
            print("Server shutting down.")
            break
        except Exception as e:
            print(f"Unexpected error: {e}")
            # 可选：稍后重试或记录错误

import datetime

def get_current_time():
    """获取当前时间并格式化为字符串"""
    return datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")

if __name__ == "__main__":
    # 启动日志
    print(f"[{get_current_time()}] Starting Logger...")
    
    try:
        pipe_server()
    except BaseException as e:
        # 异常日志
        print(f"[{get_current_time()}] Unexpected Exception: {str(e)}")
        raise
    finally:
        conn.close()
        # 终止日志
        print(f"[{get_current_time()}] Stopped")

# import PythonServiceFramework
# from PythonServiceFramework import SERVICE_CONTROL_SHUTDOWN, SERVICE_CONTROL_STOP, SERVICE_RUNNING, SERVICE_ACCEPT_STOP, SERVICE_ACCEPT_SHUTDOWN, SERVICE_STOP_PENDING, SERVICE_STOPPED
# def main(modify_status: Callable) -> int:
#     modify_status(SERVICE_RUNNING, 0, 0, SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN)
#     pipe_server()
#     conn.close()

# def my_service_control(dwControl: DWORD, modify_status: Callable) -> None:
#     if dwControl == SERVICE_CONTROL_STOP:
#         print("Stop command received")
#         modify_status(SERVICE_STOP_PENDING, 1, 16384)
#         # 这里可以添加清理代码，例如关闭数据库连接、释放资源等
#         try:
#             if 'conn' in globals() and conn:  # 检查数据库连接是否存在
#                 conn.close()
#                 print("Database connection closed")
#         except Exception as e:
#             print(f"Error closing database connection: {e}")
#         time.sleep(1)
#         modify_status(SERVICE_STOPPED)
#         time.sleep(1)
#     elif dwControl == SERVICE_CONTROL_SHUTDOWN:
#         print("Shutdown command received")
#         modify_status(SERVICE_STOPPED)
#     else:
#         print(f"Service control received: {dwControl}")
#     return

# import argparse
# if __name__ == "__main__":
#     # 解析命令行参数，从 --service-name 获取服务名称
#     parser = argparse.ArgumentParser(description="WebDAV Service")
#     parser.add_argument('--service-name', type=str, help='Service name')
#     # 有一个 install=<服务名> 参数，表示安装服务
#     parser.add_argument('--install', type=str, help='Install the service')
#     # 有一个 uninstall=<服务名> 参数，表示卸载服务
#     parser.add_argument('--uninstall', type=str, help='Uninstall the service')
#     args = parser.parse_args()

#     # 如果提供了服务名称，则进入服务模式
#     if args.service_name:
#         # 启动服务
#         PythonServiceFramework.StartDispatch(
#             svc_name=args.service_name,
#             service_thread=main,
#             service_control=my_service_control  # TODO: Implement service control handler if needed
#         )
#         # 结束进程
#         exit(0)
#     # 否则，进入管理模式
#     else:
#         # 根据 --install 和 --uninstall 参数来安装或卸载服务
#         if args.install:
#             # 安装服务
#             print("Installing service...")
#             # 这里可以调用安装服务的函数
            
#             # 通过 OpenSCManagerW 创建服务管理器
#             ctypes.windll.advapi32.OpenSCManagerW.restype = ctypes.c_void_p  # Ensure correct return type
#             scm = ctypes.windll.advapi32.OpenSCManagerW(None, None, 983103)  # SC_MANAGER_ALL_ACCESS
#             if scm == 0:  # Check if the handle is invalid (0 indicates failure)
#                 raise ctypes.WinError(ctypes.get_last_error())
#             print(f"SCM handle: {scm}")
#             # 找出 python.exe 的路径
#             python_path = sys.executable
#             # 将 python.exe 替换为 pythonw.exe
#             python_path = re.sub(r'python\.exe$', 'pythonw.exe', python_path, flags=re.IGNORECASE)
#             # CreateService
#             # 指定CreateServiceW的参数类型
#             ctypes.windll.advapi32.CreateServiceW.argtypes = [
#                 ctypes.c_void_p,  # SCM handle
#                 ctypes.c_wchar_p,  # Service name
#                 ctypes.c_wchar_p,  # Display name
#                 ctypes.c_uint,  # Desired access
#                 ctypes.c_uint,  # Service type
#                 ctypes.c_uint,  # Start type
#                 ctypes.c_uint,  # Error control
#                 ctypes.c_wchar_p,  # Binary path
#                 ctypes.c_wchar_p,  # Load order group
#                 ctypes.POINTER(ctypes.c_uint),  # Tag ID
#                 ctypes.c_wchar_p,  # Dependencies
#                 ctypes.c_wchar_p,  # Service start name
#                 ctypes.c_wchar_p  # Password
#             ]
#             ctypes.windll.advapi32.CloseServiceHandle.argtypes = [ctypes.c_void_p]
#             scHandle = ctypes.windll.advapi32.CreateServiceW(
#                 scm,
#                 args.install,
#                 args.install,
#                 983551,  # SERVICE_ALL_ACCESS
#                 0x10,  # SERVICE_WIN32_OWN_PROCESS
#                 3, # SERVICE_DEMAND_START
#                 1, # SERVICE_ERROR_NORMAL
#                 f'"{python_path}" "{__file__}" --service-name={args.install}',
#                 None, None, None, None, None
#             )
#             if not scHandle:
#                 print(f"Failed to create service '{args.install}'. Error code: {ctypes.GetLastError()}")
#                 ctypes.windll.advapi32.CloseServiceHandle(scm)
#                 raise ctypes.WinError()
#             ctypes.windll.advapi32.CloseServiceHandle(scHandle)
#             ctypes.windll.advapi32.CloseServiceHandle(scm)
#             print(f"Service '{args.install}' installed successfully.")
#         elif args.uninstall:
#             # 卸载服务
#             print("Uninstalling service...")
#             # 这里可以调用卸载服务的函数
#             command = f'sc delete {args.uninstall}'
#             result = os.system(command)
#             if result == 0:
#                 print(f"Service '{args.uninstall}' uninstalled successfully.")
#             else:
#                 print(f"Failed to uninstall service '{args.uninstall}'.")
#         else:
#             # 如果没有提供 --install 或 --uninstall 参数，则打印帮助信息
#             parser.print_help()
#             exit(1)