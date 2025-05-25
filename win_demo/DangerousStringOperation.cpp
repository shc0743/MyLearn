//: DangerousStringOperation.cpp
// Demonstration of Dangerous String Operation.
// {ThrowsException}
#include <Windows.h>
#include <cstdio>
int main() {
    const char* str1 = "123456";
    char str2[10] = "456789012";
    strcat(str2, str1); // Dangerous! (The bad behavior is HIDDEN and MAY NOT cause a crash (so you may never found it until you use it into production environment))
    puts(str2);
    // safer way (will throw exception if overflow)
    char str3[10] = "456789012";
    strcat_s(str3, str1); // Crash here! 0xC0000409
    // But only usable in MSVC
    puts(str3); // Never reaches here
    #if 0
    // in fact strcat_s uses as follows:
template <size_t size>
errno_t strcat_s(
   char (&strDestination)[size],
   const char *strSource
); // C++ only
    // If uses in C, or the buffer is dynamically allocated (e.g. C++ `new`)
    // You will have to use
errno_t strcat_s(
   char *strDestination,
   size_t numberOfElements,
   const char *strSource
);
    #endif
    return 0;
} /* Returns: -1073740791 (NTSTATUS 0xC0000409: 系统在此应用程序中检测到基于堆栈的缓冲区溢出。溢出可能允许恶意用户获得此应用程序的控制。  ) *///:~