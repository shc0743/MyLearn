#include <chrono>
#include <cstdint>
#include <emscripten.h>
#include <emscripten/bind.h>

extern "C" {

// 获取自纪元以来的纳秒数
uint64_t EMSCRIPTEN_KEEPALIVE get_nanoseconds_since_epoch() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
}

// 获取自纪元以来的微秒数
uint64_t EMSCRIPTEN_KEEPALIVE get_microseconds_since_epoch() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

// 获取自纪元以来的毫秒数
uint64_t EMSCRIPTEN_KEEPALIVE get_milliseconds_since_epoch() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
}

// 创建高精度计时器实例
uint64_t EMSCRIPTEN_KEEPALIVE create_timer() {
    auto start_time = new std::chrono::high_resolution_clock::time_point(
        std::chrono::high_resolution_clock::now()
    );
    return reinterpret_cast<uint64_t>(start_time);
}

// 获取经过的纳秒数
uint64_t EMSCRIPTEN_KEEPALIVE get_elapsed_nanoseconds(uint64_t timer_handle) {
    auto start_time = reinterpret_cast<std::chrono::high_resolution_clock::time_point*>(timer_handle);
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(current_time - *start_time);
    return elapsed.count();
}

// 重置计时器
void EMSCRIPTEN_KEEPALIVE reset_timer(uint64_t timer_handle) {
    auto start_time = reinterpret_cast<std::chrono::high_resolution_clock::time_point*>(timer_handle);
    *start_time = std::chrono::high_resolution_clock::now();
}

// 销毁计时器
void EMSCRIPTEN_KEEPALIVE destroy_timer(uint64_t timer_handle) {
    auto start_time = reinterpret_cast<std::chrono::high_resolution_clock::time_point*>(timer_handle);
    delete start_time;
}

}