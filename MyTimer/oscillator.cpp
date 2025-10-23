#include <emscripten.h>
#include <stdint.h>
#include <string.h>
#include "./third-party/sha256.h"

// 性能密集型计算 - 使用SHA256作为振荡器
extern "C" double EMSCRIPTEN_KEEPALIVE intensive_calculation(int iterations) {
    struct sha256_buff ctx;
    uint8_t hash[32];
    char data[64];
    double result = 0.0;
    
    // 填充测试数据
    for (int i = 0; i < 64; i++) {
        data[i] = (char)(i % 256);
    }
    
    for (int i = 0; i < iterations; i++) {
        // 多次SHA256计算来消耗时间
        for (int j = 0; j < 10; j++) {
            sha256_init(&ctx);
            
            // 更新不同的数据确保每次计算都不同
            data[0] = (char)((i + j) % 256);
            data[1] = (char)((i * j) % 256);
            
            sha256_update(&ctx, data, 64);
            sha256_finalize(&ctx);
            sha256_read(&ctx, hash);
            
            // 使用哈希值的一部分来避免被优化掉
            result += hash[0] + hash[1];
        }
    }
    
    return result;
}

// 精确校准函数
extern "C" double EMSCRIPTEN_KEEPALIVE calibrate_oscillator(int calibration_time_ms) {
    auto start = emscripten_get_now();
    auto end = start + calibration_time_ms;
    
    int iterations = 0;
    double dummy_result = 0.0;
    
    // 运行固定时间，统计迭代次数
    while (emscripten_get_now() < end) {
        dummy_result += intensive_calculation(1); // 每次1次迭代（包含10次SHA256）
        iterations++;
    }
    
    // 返回每毫秒的迭代次数（振荡频率）
    return static_cast<double>(iterations) / calibration_time_ms;
}

// 高精度计时函数
extern "C" double EMSCRIPTEN_KEEPALIVE measure_duration_oscillator(
    int target_duration_ms, 
    double iterations_per_ms
) {
    int target_iterations = static_cast<int>(target_duration_ms * iterations_per_ms);
    
    auto start_js = emscripten_get_now();
    
    // 执行精确次数的SHA256计算
    double result = intensive_calculation(target_iterations);
    
    auto end_js = emscripten_get_now();
    double actual_duration = end_js - start_js;
    
    // 返回实际测量时间（用于验证）
    return actual_duration;
}