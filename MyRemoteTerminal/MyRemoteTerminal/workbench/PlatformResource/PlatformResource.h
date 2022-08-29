
#ifdef  _WIN64
#include "PlatformResource.x64.h"

#define IDR_BIN_7z IDR_BIN_7z_x64
#define IDR_BIN_rsah IDR_BIN_rsah_x64
#define IDR_BIN_rauth IDR_BIN_rauth_x64

#else
#include "PlatformResource.x86.h"

#define IDR_BIN_7z IDR_BIN_7z_x86
#define IDR_BIN_rsah IDR_BIN_rsah_x86
#define IDR_BIN_rauth IDR_BIN_rauth_x86

#endif //  _WIN64


