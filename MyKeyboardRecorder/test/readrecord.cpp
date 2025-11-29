#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <iomanip>

#pragma pack(push, 1)
struct RecordItem {
    uint64_t t;
    uint8_t modifier;
    uint8_t reserved;
    uint8_t keycode[6];
};
#pragma pack(pop)

int main(int argc, char* argv[]) {
    // 检查参数
    if (argc != 2) {
        std::cerr << "使用方法: " << argv[0] << " <文件名>" << std::endl;
        return 1;
    }

    const char* filename = argv[1];

    // 打开文件
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return 1;
    }

    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streamsize file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    // 检查文件大小是否合理
    if (file_size % sizeof(RecordItem) != 0) {
        std::cerr << "警告: 文件大小不是RecordItem结构的整数倍" << std::endl;
    }

    // 计算记录数量
    size_t record_count = file_size / sizeof(RecordItem);

    std::cout << "文件大小: " << file_size << " 字节" << std::endl;
    std::cout << "记录数量: " << record_count << std::endl;
    std::cout << std::endl;

    // 读取所有记录
    std::vector<RecordItem> records(record_count);
    if (!file.read(reinterpret_cast<char*>(records.data()), file_size)) {
        std::cerr << "读取文件失败" << std::endl;
        return 1;
    }

    file.close();

    // 打印表头
    std::cout << std::left 
              << std::setw(16) << "时间戳(t)" 
              << std::setw(12) << "修饰键(modifier)" 
              << std::setw(10) << "保留(reserved)" 
              << "键码(keycode)" 
              << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    // 打印每条记录
    for (size_t i = 0; i < records.size(); ++i) {
        const RecordItem& item = records[i];
        
        std::cout << std::left 
                  << std::setw(16) << item.t 
                  << std::setw(12) << static_cast<int>(item.modifier)
                  << std::setw(10) << static_cast<int>(item.reserved);
        
        // 打印keycode数组
        std::cout << "[";
        for (int j = 0; j < 6; ++j) {
            std::cout << static_cast<int>(item.keycode[j]);
            if (j < 5) std::cout << ", ";
        }
        std::cout << "]" << std::endl;
    }

    return 0;
}