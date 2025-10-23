#!/bin/bash

# 清理旧文件
rm -f timer.js timer.wasm 2>/dev/null

# 编译 WASM 版本
em++ -O3 \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_get_nanoseconds_since_epoch", "_get_microseconds_since_epoch", "_get_milliseconds_since_epoch", "_create_timer", "_get_elapsed_nanoseconds", "_reset_timer", "_destroy_timer", "_malloc", "_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall", "cwrap"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s MODULARIZE \
    -s EXPORT_NAME=timer \
    -o timer.js \
    timer.cpp
    
em++ -O3 \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS='["_intensive_calculation", "_calibrate_oscillator"]' \
    -s EXPORTED_RUNTIME_METHODS='[]' \
    -s MODULARIZE=1 \
    -s EXPORT_NAME='oscillator' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -o oscillator.js \
    oscillator.cpp ./third-party/sha256.c
    
# Linux/macOS
g++ -std=c++17 app.cpp

echo "Timer compiled."
