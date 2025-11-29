#include "vk2hid.hpp"
std::unordered_map<int, uint8_t> VK_TO_HID = {
	// 字母 A-Z: HID 0x04 - 0x1D
	{ 'A', 0x04 },{ 'B', 0x05 },{ 'C', 0x06 },{ 'D', 0x07 },{ 'E', 0x08 },
	{ 'F', 0x09 },{ 'G', 0x0A },{ 'H', 0x0B },{ 'I', 0x0C },{ 'J', 0x0D },
	{ 'K', 0x0E },{ 'L', 0x0F },{ 'M', 0x10 },{ 'N', 0x11 },{ 'O', 0x12 },
	{ 'P', 0x13 },{ 'Q', 0x14 },{ 'R', 0x15 },{ 'S', 0x16 },{ 'T', 0x17 },
	{ 'U', 0x18 },{ 'V', 0x19 },{ 'W', 0x1A },{ 'X', 0x1B },{ 'Y', 0x1C },
	{ 'Z', 0x1D },

	// 顶排数字 1..0: HID 0x1E - 0x27
	{ '1', 0x1E },{ '2', 0x1F },{ '3', 0x20 },{ '4', 0x21 },{ '5', 0x22 },
	{ '6', 0x23 },{ '7', 0x24 },{ '8', 0x25 },{ '9', 0x26 },{ '0', 0x27 },

	// 常用控制键
	{ VK_RETURN,  0x28 }, // Enter
	{ VK_ESCAPE,  0x29 }, // Esc
	{ VK_BACK,    0x2A }, // Backspace (Delete backward)
	{ VK_TAB,     0x2B }, // Tab
	{ VK_SPACE,   0x2C }, // Space
	{ VK_OEM_MINUS,0x2D }, // - _
	{ VK_OEM_PLUS, 0x2E }, // = +
	{ VK_OEM_4,    0x2F }, // [ {
	{ VK_OEM_6,    0x30 }, // ] }
	{ VK_OEM_5,    0x31 }, // \ |
	{ VK_OEM_7,    0x34 }, // ' "
	{ VK_OEM_1,    0x33 }, // ; :
	{ VK_OEM_3,    0x35 }, // ` ~
	{ VK_OEM_COMMA,0x36 }, // , <
	{ VK_OEM_PERIOD,0x37 },// . >
	{ VK_OEM_2,    0x38 }, // / ?
	{ VK_CAPITAL,  0x39 }, // Caps Lock

	// 功能键 F1 - F12: HID 0x3A - 0x45
	{ VK_F1,  0x3A }, { VK_F2,  0x3B }, { VK_F3,  0x3C }, { VK_F4,  0x3D },
	{ VK_F5,  0x3E }, { VK_F6,  0x3F }, { VK_F7,  0x40 }, { VK_F8,  0x41 },
	{ VK_F9,  0x42 }, { VK_F10, 0x43 }, { VK_F11, 0x44 }, { VK_F12, 0x45 },

	// PrintScreen / ScrollLock / Pause
	{ VK_SNAPSHOT, 0x46 }, // Print Screen / SysRq
	{ VK_SCROLL,   0x47 }, // Scroll Lock
	{ VK_PAUSE,    0x48 }, // Pause

	// 编辑/导航键
	{ VK_INSERT, 0x49 }, { VK_HOME, 0x4A }, { VK_PRIOR, 0x4B }, // Page Up
	{ VK_DELETE, 0x4C }, { VK_END,  0x4D }, { VK_NEXT,  0x4E }, // Page Down

	// 方向键
	{ VK_RIGHT, 0x4F }, { VK_LEFT, 0x50 }, { VK_DOWN, 0x51 }, { VK_UP, 0x52 },

	// 小键盘与 NumLock
	{ VK_NUMLOCK, 0x53 },
	{ VK_DIVIDE,  0x54 }, // Keypad /
	{ VK_MULTIPLY,0x55 }, // Keypad *
	{ VK_SUBTRACT,0x56 }, // Keypad -
	{ VK_ADD,     0x57 }, // Keypad +
	// 小键盘 Enter（注意：Keypad Enter 与主键盘 Enter 不同）
	{ VK_SEPARATOR /* sometimes used */, 0x58 },
	{ VK_RETURN /* numeric enter may be different in some drivers */, 0x28 },

	// Keypad 1..0 and .  (HID: 0x59 - 0x63)
	{ VK_NUMPAD1, 0x59 }, { VK_NUMPAD2, 0x5A }, { VK_NUMPAD3, 0x5B },
	{ VK_NUMPAD4, 0x5C }, { VK_NUMPAD5, 0x5D }, { VK_NUMPAD6, 0x5E },
	{ VK_NUMPAD7, 0x5F }, { VK_NUMPAD8, 0x60 }, { VK_NUMPAD9, 0x61 },
	{ VK_NUMPAD0, 0x62 }, { VK_DECIMAL, 0x63 },

	// 额外常见按键（媒体/应用键等可按需扩展）
	{ VK_APPS,   0x65 }, // Application/Menu key (映射到 HID usage if supported)
};
