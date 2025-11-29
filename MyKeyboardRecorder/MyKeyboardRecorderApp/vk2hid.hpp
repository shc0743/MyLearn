#pragma once
#include <Windows.h>
#include <unordered_map>
// --- 完整 VK -> HID 映射表（常用键/功能键/数字/标点/小键盘等） ---
// 映射基于 USB HID Keyboard/Keypad Usage Page（Usage IDs）.
extern std::unordered_map<int, uint8_t> VK_TO_HID;
