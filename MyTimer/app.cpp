#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <iomanip>
#include <csignal>
#include <termios.h>
#include <unistd.h>

using namespace std;
using namespace std::chrono;

// 原子标志位用于控制程序退出
atomic<bool> g_running(true);

// 信号处理函数
void signal_handler(int signal) {
    g_running = false;
    cout << "\n程序退出中..." << endl;
}

// 跨平台的按键检测函数（非阻塞）
char getch() {
    char buf = 0;
    struct termios old = {0};
    
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    
    if (read(0, &buf, 1) < 0)
        perror("read()");
    
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    
    return buf;
}

// 检查是否有按键输入（非阻塞）
bool kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }
    
    return false;
}

// 显示当前时间戳
void show_current_timestamp() {
    auto now = high_resolution_clock::now();
    auto ns = duration_cast<nanoseconds>(now.time_since_epoch()).count();
    auto us = duration_cast<microseconds>(now.time_since_epoch()).count();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
    
    // 转换为可读时间
    auto system_time = system_clock::now();
    time_t time = system_clock::to_time_t(system_time);
    
    cout << "\n=== 当前时间戳 ===" << endl;
    cout << "本地时间: " << ctime(&time);
    cout << "毫秒: " << ms << " ms" << endl;
    cout << "微秒: " << us << " us" << endl;
    cout << "纳秒: " << ns << " ns" << endl;
    cout << "==================" << endl;
}

// 计时器功能
void start_timer() {
    cout << "\n计时器已启动... 按任意键停止计时" << endl;
    
    auto start_time = high_resolution_clock::now();
    
    // 等待用户按键
    while (!kbhit() && g_running) {
        this_thread::sleep_for(milliseconds(10));
    }
    
    if (!g_running) return;
    
    // 读取按键（清空输入缓冲区）
    getch();
    
    auto end_time = high_resolution_clock::now();
    auto dd = end_time - start_time;
    
    auto ns = duration_cast<nanoseconds>(dd).count();
    auto us = duration_cast<microseconds>(dd).count();
    auto ms = duration_cast<milliseconds>(dd).count();
    auto seconds = duration_cast<duration<double>>(dd).count();
    
    cout << "\n=== 计时结果 ===" << endl;
    cout << fixed << setprecision(9);
    cout << "总时间: " << seconds << " 秒" << endl;
    cout << setprecision(6);
    cout << "毫秒: " << ms << " ms" << endl;
    cout << "微秒: " << us << " us" << endl;
    cout << "纳秒: " << ns << " ns" << endl;
    cout << "================" << endl;
}

// 显示帮助信息
void show_help() {
    cout << "\n=== 使用说明 ===" << endl;
    cout << "按 'g' 或 'G': 显示当前时间戳" << endl;
    cout << "按其他任意键: 开始/停止计时" << endl;
    cout << "按 Ctrl+C: 退出程序" << endl;
    cout << "================" << endl;
}

int main() {
    // 设置信号处理
    signal(SIGINT, signal_handler);
    
    cout << "高精度计时器程序" << endl;
    cout << "使用 C++ chrono 库实现纳秒级精度" << endl;
    show_help();
    
    while (g_running) {
        cout << "\n请按键 (g-显示时间戳, 其他键-开始计时, Ctrl+C-退出): ";
        
        // 等待用户输入
        char ch = getch();
        
        if (!g_running) break;
        
        if (ch == 'g' || ch == 'G') {
            show_current_timestamp();
        } else {
            start_timer();
        }
    }
    
    cout << "程序已退出" << endl;
    return 0;
}