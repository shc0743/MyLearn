#include "ch9329.hpp"

std::vector<uint8_t> replay::devices::CH9329::build_kb_frame(uint8_t addr, uint8_t modifier, const uint8_t keycodes[6]) {
	// CH9329 frame 结构:
	// 0: 0x57
	// 1: 0xAB
	// 2: addr
	// 3: cmd = 0x02 (keyboard)
	// 4: len = 8
	// 5: modifier
	// 6: reserved (0)
	// 7..12: 6 bytes keycode
	// 13: checksum = sum(frame[0:13]) % 256

	std::vector<uint8_t> frame;
	frame.reserve(14); // 预分配空间

	// 添加帧头和数据
	frame.push_back(0x57);
	frame.push_back(0xAB);
	frame.push_back(addr);
	frame.push_back(CMD_KB);
	frame.push_back(8);        // 数据长度
	frame.push_back(modifier);
	frame.push_back(0);        // reserved

	// 添加6个键码字节
	for (int i = 0; i < 6; ++i) {
		frame.push_back(keycodes[i]);
	}

	// 计算校验和
	uint8_t checksum = 0;
	for (size_t i = 0; i < frame.size(); ++i) {
		checksum = (checksum + frame[i]) & 0xFF;
	}
	frame.push_back(checksum);

	return frame;
}