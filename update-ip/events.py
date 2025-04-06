import win32evtlog
import win32evtlogutil
import win32security
import win32con
import win32api

import win32event
import win32api
import win32con
import win32evtlog
import win32evtlogutil
import win32security
import sys

def write_to_event_log(log_name, source_name, event_id, event_type, message):
    """
    写入Windows事件日志
    
    参数:
        log_name: 日志名称，如"Application"
        source_name: 事件源名称
        event_id: 事件ID
        event_type: 事件类型(win32con.EVENTLOG_INFORMATION_TYPE等)
        message: 要记录的消息
    """
    # 获取当前用户的安全标识符
    user_sid = win32security.GetTokenInformation(
        win32security.OpenProcessToken(win32api.GetCurrentProcess(), win32con.TOKEN_READ),
        win32security.TokenUser
    )[0]

    # 自动处理 message 参数：
    # - 如果 message 是字符串，则转换为 [message]
    # - 如果 message 已经是列表或元组，则直接使用
    strings = [message] if isinstance(message, str) else list(message)
    
    # 写入事件日志
    win32evtlogutil.ReportEvent(
        source_name,
        event_id,
        eventType=event_type,
        strings=strings,
        data=None,
        sid=user_sid,
    )
