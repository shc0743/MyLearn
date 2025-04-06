 // update-ip.mc
 // update-ip 服务消息定义文件
 // 严重性定义
 // 设施定义
 // 语言定义
 // 消息类别定义
//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//
#define FACILITY_SYSTEM                  0x0


//
// Define the severity codes
//
#define STATUS_SEVERITY_SUCCESS          0x0


//
// MessageId: SERVICE_CATEGORY
//
// MessageText:
//
// 服务事件
//
#define SERVICE_CATEGORY                 ((WORD)0x00000001L)

 // 消息定义
 // 服务状态消息
//
// MessageId: MSG_SERVICE_STARTED
//
// MessageText:
//
// update-ip 服务已启动。
//
#define MSG_SERVICE_STARTED              ((DWORD)0x00000033L)

//
// MessageId: MSG_SERVICE_STOPPED
//
// MessageText:
//
// update-ip 服务已停止。
//
#define MSG_SERVICE_STOPPED              ((DWORD)0x00000034L)

//
// MessageId: MSG_SERVICE_PAUSED
//
// MessageText:
//
// update-ip 服务已暂停。
//
#define MSG_SERVICE_PAUSED               ((DWORD)0x00000035L)

//
// MessageId: MSG_SERVICE_CONTINUED
//
// MessageText:
//
// update-ip 服务已继续。
//
#define MSG_SERVICE_CONTINUED            ((DWORD)0x00000036L)

//
// MessageId: MSG_SERVICE_START_PENDING
//
// MessageText:
//
// update-ip 服务正在启动...
//
#define MSG_SERVICE_START_PENDING        ((DWORD)0x00000037L)

//
// MessageId: MSG_SERVICE_STOPP_PENDING
//
// MessageText:
//
// update-ip 服务正在停止...
//
#define MSG_SERVICE_STOPP_PENDING        ((DWORD)0x00000038L)

//
// MessageId: MSG_SERVICE_PAUSE_PENDING
//
// MessageText:
//
// update-ip 服务正在暂停...
//
#define MSG_SERVICE_PAUSE_PENDING        ((DWORD)0x00000039L)

//
// MessageId: MSG_SERVICE_CONTINUE_PENDING
//
// MessageText:
//
// update-ip 服务正在继续...
//
#define MSG_SERVICE_CONTINUE_PENDING     ((DWORD)0x0000003AL)

 // 错误消息
//
// MessageId: MSG_SERVICE_ERROR
//
// MessageText:
//
// update-ip 服务遇到问题。%1
//
#define MSG_SERVICE_ERROR                ((DWORD)0x0000003BL)

 // IP 相关消息
//
// MessageId: MSG_IP_DETECTED
//
// MessageText:
//
// IP 地址已获取: %1
//
#define MSG_IP_DETECTED                  ((DWORD)0x00000014L)

//
// MessageId: MSG_IP_CHANGED
//
// MessageText:
//
// 检测到 IP 地址变更: %1 -> %2
//
#define MSG_IP_CHANGED                   ((DWORD)0x00000015L)

//
// MessageId: MSG_IP_UPDATE_SCHEDULED
//
// MessageText:
//
// update-ip 任务已规划。预计执行时间：%1
//
#define MSG_IP_UPDATE_SCHEDULED          ((DWORD)0x00000016L)

//
// MessageId: MSG_IP_UPDATE_STARTED
//
// MessageText:
//
// update-ip 任务已开始。
//
#define MSG_IP_UPDATE_STARTED            ((DWORD)0x00000017L)

//
// MessageId: MSG_IP_UPDATE_SUCCESS
//
// MessageText:
//
// IP 地址成功更新。
//
#define MSG_IP_UPDATE_SUCCESS            ((DWORD)0x00000018L)

//
// MessageId: MSG_IP_UPDATE_FAILURE
//
// MessageText:
//
// IP 地址未能成功更新。
//
#define MSG_IP_UPDATE_FAILURE            ((DWORD)0x00000019L)

//
// MessageId: MSG_IP_UPDATE_FAILURE_WITH_DATA
//
// MessageText:
//
// IP 地址未能成功更新。%1
//
#define MSG_IP_UPDATE_FAILURE_WITH_DATA  ((DWORD)0x00000020L)

 // 自定义消息
//
// MessageId: MSG_CUSTOM
//
// MessageText:
//
// %1
//
#define MSG_CUSTOM                       ((DWORD)0x000003E8L)

 // 广告消息
//
// MessageId: MSG_FURINA1013
//
// MessageText:
//
// Genshin Impact: https://ys.mihoyo.com/ See also: https://www.bilibili.com/video/BV1ZG411X71k/
//
#define MSG_FURINA1013                   ((DWORD)0x000003F5L)


//
// MessageId: MSG_NEUVILLETTE1218
//
// MessageText:
//
// Genshin Impact: https://ys.mihoyo.com/ or https://genshin.hoyoverse.com/ See also: https://www.bilibili.com/video/BV1FC4y1f7hW/
//
#define MSG_NEUVILLETTE1218              ((DWORD)0x000004C2L)

//
// MessageId: MSG_REVERSE1999_GLOBAL
//
// MessageText:
//
// Reverse 1999: https://re1999.bluepoch.com/
//
#define MSG_REVERSE1999_GLOBAL           ((DWORD)0x000007CEL)

//
// MessageId: MSG_REVERSE1999
//
// MessageText:
//
// 重返未来1999: https://re.bluepoch.com/
//
#define MSG_REVERSE1999                  ((DWORD)0x000007CFL)

//
// MessageId: MSG_HKSR
//
// MessageText:
//
// Honkai: Star Rail: https://sr.mihoyo.com/ or https://hsr.hoyoverse.com/
//
#define MSG_HKSR                         ((DWORD)0x000007D0L)

