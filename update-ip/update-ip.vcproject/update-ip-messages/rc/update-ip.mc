; // update-ip.mc
; // update-ip 服务消息定义文件

; // 严重性定义
SeverityNames=(Success=0x0:STATUS_SEVERITY_SUCCESS
              )

; // 设施定义
FacilityNames=(System=0x0:FACILITY_SYSTEM
              )

; // 语言定义
LanguageNames=(Chinese=0x804:MSG00804)

; // 消息类别定义
MessageIdTypedef=WORD

MessageId=0x1
SymbolicName=SERVICE_CATEGORY
Language=Chinese
服务事件
.

; // 消息定义
MessageIdTypedef=DWORD

; // 服务状态消息
MessageId=0x33
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_STARTED
Language=Chinese
update-ip 服务已启动。
.

MessageId=0x34
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_STOPPED
Language=Chinese
update-ip 服务已停止。
.

MessageId=0x35
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_PAUSED
Language=Chinese
update-ip 服务已暂停。
.

MessageId=0x36
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_CONTINUED
Language=Chinese
update-ip 服务已继续。
.

MessageId=0x37
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_START_PENDING
Language=Chinese
update-ip 服务正在启动...
.

MessageId=0x38
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_STOPP_PENDING
Language=Chinese
update-ip 服务正在停止...
.

MessageId=0x39
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_PAUSE_PENDING
Language=Chinese
update-ip 服务正在暂停...
.

MessageId=0x3A
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_CONTINUE_PENDING
Language=Chinese
update-ip 服务正在继续...
.

; // 错误消息
MessageId=0x3B
Severity=Success
Facility=System
SymbolicName=MSG_SERVICE_ERROR
Language=Chinese
update-ip 服务遇到问题。%1
.

; // IP 相关消息
MessageId=0x14
Severity=Success
Facility=System
SymbolicName=MSG_IP_DETECTED
Language=Chinese
IP 地址已获取: %1
.

MessageId=0x15
Severity=Success
Facility=System
SymbolicName=MSG_IP_CHANGED
Language=Chinese
检测到 IP 地址变更: %1 -> %2
.

MessageId=0x16
Severity=Success
Facility=System
SymbolicName=MSG_IP_UPDATE_SCHEDULED
Language=Chinese
update-ip 任务已规划。预计执行时间：%1
.

MessageId=0x17
Severity=Success
Facility=System
SymbolicName=MSG_IP_UPDATE_STARTED
Language=Chinese
update-ip 任务已开始。
.

MessageId=0x18
Severity=Success
Facility=System
SymbolicName=MSG_IP_UPDATE_SUCCESS
Language=Chinese
IP 地址成功更新。
.

MessageId=0x19
Severity=Success
Facility=System
SymbolicName=MSG_IP_UPDATE_FAILURE
Language=Chinese
IP 地址未能成功更新。
.

MessageId=0x20
Severity=Success
Facility=System
SymbolicName=MSG_IP_UPDATE_FAILURE_WITH_DATA
Language=Chinese
IP 地址未能成功更新。%1
.

; // 自定义消息
MessageId=0x3E8
Severity=Success
Facility=System
SymbolicName=MSG_CUSTOM
Language=Chinese
%1
.

; // 广告消息
MessageId=0x3F5
Severity=Success
Facility=System
SymbolicName=MSG_FURINA1013
Language=Chinese
Genshin Impact: https://ys.mihoyo.com/ See also: https://www.bilibili.com/video/BV1ZG411X71k/
.

;
MessageId=0x4C2
Severity=Success
Facility=System
SymbolicName=MSG_NEUVILLETTE1218
Language=Chinese
Genshin Impact: https://ys.mihoyo.com/ or https://genshin.hoyoverse.com/ See also: https://www.bilibili.com/video/BV1FC4y1f7hW/
.

MessageId=0x7CE
Severity=Success
Facility=System
SymbolicName=MSG_REVERSE1999_GLOBAL
Language=Chinese
Reverse 1999: https://re1999.bluepoch.com/
.

MessageId=0x7CF
Severity=Success
Facility=System
SymbolicName=MSG_REVERSE1999
Language=Chinese
重返未来1999: https://re.bluepoch.com/
.

MessageId=0x7D0
Severity=Success
Facility=System
SymbolicName=MSG_HKSR
Language=Chinese
Honkai: Star Rail: https://sr.mihoyo.com/ or https://hsr.hoyoverse.com/
.
