#pragma once
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <Windows.h>

namespace replay::devices::CH9329 {

	// CH9329 键盘指令常量
	constexpr uint8_t CMD_KB = 0x02;
	constexpr uint8_t ADDR = 0x00;

	// 构建数据帧
	std::vector<uint8_t> build_kb_frame(uint8_t addr, uint8_t modifier, const uint8_t keycodes[6]);

	inline std::vector<uint8_t> build_kb_frame(uint8_t addr, uint8_t modifier, const std::vector<uint8_t>& keycodes) {
		if (keycodes.size() != 6) {
			throw std::invalid_argument("Keycodes must have exactly 6 elements");
		}
		return build_kb_frame(addr, modifier, keycodes.data());
	}

	template<size_t N>
	std::vector<uint8_t> build_kb_frame(uint8_t addr, uint8_t modifier, const std::array<uint8_t, N>& keycodes) {
		static_assert(N == 6, "Keycodes must have exactly 6 elements");
		return build_kb_frame(addr, modifier, keycodes.data());
	}

	inline bool SendToCH9329Device(HANDLE hCom, const std::vector<uint8_t>& data) {
		DWORD bytesWritten{};
		BOOL success = WriteFile(
			hCom,           // 串口句柄
			data.data(),    // 要发送的数据
			(DWORD)data.size(),    // 数据长度
			&bytesWritten,  // 实际写入的字节数
			NULL            // 重叠I/O（同步操作设为NULL）
		);
		FlushFileBuffers(hCom);
		return success && (bytesWritten == data.size());
	}
}