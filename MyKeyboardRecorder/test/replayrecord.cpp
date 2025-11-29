// ch9329_replay.cpp
// 用法示例：
//  Linux/macOS:  g++ -std=c++17 ch9329_replay.cpp -o ch9329_replay
//  Windows (MSVC): cl /std:c++17 ch9329_replay.cpp
//
//  运行示例： ./ch9329_replay records.bin /dev/ttyUSB0 9600
//
// 假设文件是由连续的 struct RecordItem 写入（little-endian）
// struct RecordItem {
//     uint64_t t;         // 毫秒，相对时间戳
//     uint8_t modifier;
//     uint8_t reserved;
//     uint8_t keycode[6];
// };

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <chrono>
#include <thread>
#include <optional>

#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#endif

using namespace std;

struct RecordItem {
    uint64_t t;
    uint8_t modifier;
    uint8_t reserved;
    uint8_t keycode[6];
};

// ---------- Serial helpers (simple read/write + timeout) ----------
#ifdef _WIN32

struct SerialHandle {
    HANDLE h = INVALID_HANDLE_VALUE;
};

optional<SerialHandle> openSerial(const string& portName, unsigned int baud) {
    // portName: "COM3" or "\\\\.\\COM10"
    string full = portName;
    if (portName.rfind("\\\\.\\", 0) != 0 && portName.size() > 3 && portName.substr(0, 3) == "COM") {
        // for COM10+ usually need \\.\ prefix on Windows, but often passing "COM3" works.
        if (portName.size() >= 4) full = "\\\\.\\" + portName;
    }
    HANDLE h = CreateFileA(full.c_str(), GENERIC_READ | GENERIC_WRITE,
        0, nullptr, OPEN_EXISTING, 0, nullptr);
    if (h == INVALID_HANDLE_VALUE) {
        cerr << "OpenSerial: CreateFile failed, error=" << GetLastError() << "\n";
        return nullopt;
    }

    DCB dcb;
    SecureZeroMemory(&dcb, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    if (!GetCommState(h, &dcb)) {
        cerr << "GetCommState failed\n";
        CloseHandle(h);
        return nullopt;
    }
    dcb.BaudRate = baud;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fBinary = TRUE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;

    if (!SetCommState(h, &dcb)) {
        cerr << "SetCommState failed\n";
        CloseHandle(h);
        return nullopt;
    }

    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 500; // ms
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 500;
    SetCommTimeouts(h, &timeouts);

    SerialHandle sh; sh.h = h;
    return sh;
}

bool serialWrite(const SerialHandle& sh, const vector<uint8_t>& bytes) {
    DWORD written = 0;
    if (!WriteFile(sh.h, bytes.data(), (DWORD)bytes.size(), &written, nullptr)) {
        cerr << "WriteFile failed, err=" << GetLastError() << "\n";
        return false;
    }
    return written == bytes.size();
}

vector<uint8_t> serialReadWithTimeout(const SerialHandle& sh, int timeout_ms, size_t maxBytes) {
    vector<uint8_t> buf;
    buf.reserve(maxBytes);
    DWORD start = GetTickCount();
    while ((int)(GetTickCount() - start) < timeout_ms && buf.size() < maxBytes) {
        DWORD avail = 0;
        COMSTAT cs;
        DWORD err;
        if (!ClearCommError(sh.h, &err, &cs)) break;
        if (cs.cbInQue == 0) {
            Sleep(5);
            continue;
        }
        DWORD toRead = (DWORD)min<size_t>(cs.cbInQue, maxBytes - buf.size());
        vector<uint8_t> tmp(toRead);
        DWORD r = 0;
        if (!ReadFile(sh.h, tmp.data(), toRead, &r, nullptr)) break;
        tmp.resize(r);
        buf.insert(buf.end(), tmp.begin(), tmp.end());
    }
    return buf;
}

void closeSerial(SerialHandle& sh) {
    if (sh.h != INVALID_HANDLE_VALUE) CloseHandle(sh.h);
    sh.h = INVALID_HANDLE_VALUE;
}

#else  // POSIX

struct SerialHandle {
    int fd = -1;
};

static speed_t baudToFlag(unsigned int baud) {
    switch (baud) {
    case 1200: return B1200;
    case 2400: return B2400;
    case 4800: return B4800;
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
    default: return B9600; // fallback
    }
}

optional<SerialHandle> openSerial(const string& portPath, unsigned int baud) {
    int fd = open(portPath.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("open");
        return nullopt;
    }
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return nullopt;
    }
    cfmakeraw(&tty);
    speed_t sp = baudToFlag(baud);
    cfsetospeed(&tty, sp);
    cfsetispeed(&tty, sp);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 0;
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return nullopt;
    }
    SerialHandle sh; sh.fd = fd;
    return sh;
}

bool serialWrite(const SerialHandle& sh, const vector<uint8_t>& bytes) {
    ssize_t w = write(sh.fd, bytes.data(), bytes.size());
    if (w < 0) {
        perror("write");
        return false;
    }
    return (size_t)w == bytes.size();
}

vector<uint8_t> serialReadWithTimeout(const SerialHandle& sh, int timeout_ms, size_t maxBytes) {
    vector<uint8_t> out;
    out.reserve(maxBytes);
    fd_set rfds;
    struct timeval tv;
    int totalWait = 0;
    const int step = 10; // ms
    while (totalWait < timeout_ms && out.size() < maxBytes) {
        FD_ZERO(&rfds);
        FD_SET(sh.fd, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = step * 1000;
        int rv = select(sh.fd + 1, &rfds, nullptr, nullptr, &tv);
        if (rv > 0 && FD_ISSET(sh.fd, &rfds)) {
            uint8_t buf[256];
            ssize_t r = read(sh.fd, buf, sizeof(buf));
            if (r > 0) {
                size_t toCopy = (size_t)r;
                if (out.size() + toCopy > maxBytes) toCopy = maxBytes - out.size();
                out.insert(out.end(), buf, buf + toCopy);
            }
            else if (r < 0) {
                perror("read");
                break;
            }
            else {
                // 0 bytes, continue
            }
        }
        totalWait += step;
    }
    return out;
}

void closeSerial(SerialHandle& sh) {
    if (sh.fd >= 0) close(sh.fd);
    sh.fd = -1;
}

#endif

// ---------- CH9329 frame builder ----------
vector<uint8_t> buildKbFrame(uint8_t addr, uint8_t cmd, const uint8_t data[], size_t dataLen) {
    vector<uint8_t> frame;
    frame.reserve(2 + 1 + 1 + 1 + dataLen + 1);
    // HEAD
    frame.push_back(0x57);
    frame.push_back(0xAB);
    // ADDR
    frame.push_back(addr);
    // CMD
    frame.push_back(cmd);
    // LEN
    frame.push_back((uint8_t)dataLen);
    // DATA
    for (size_t i = 0; i < dataLen; i++) frame.push_back(data[i]);
    // SUM = sum of HEAD+ADDR+CMD+LEN+DATA (lowest 8 bits)
    uint32_t s = 0;
    for (auto b : frame) s += b;
    uint8_t sum = (uint8_t)(s & 0xFF);
    frame.push_back(sum);
    return frame;
}

bool expectAck(const vector<uint8_t>& resp, uint8_t origCmd) {
    // success response command = origCmd | 0x80 (see doc); length usually 1 and data = status
    if (resp.size() < 7) return false; // minimal ack frame size
    // find HEAD 0x57 0xAB at start
    if (resp[0] != 0x57 || resp[1] != 0xAB) return false;
    uint8_t addr = resp[2];
    uint8_t cmd = resp[3];
    uint8_t len = resp[4];
    if (cmd != (uint8_t)(origCmd | 0x80) && cmd != (uint8_t)(origCmd | 0xC0)) {
        return false;
    }
    // status is data[0] if len >=1
    if (len >= 1) {
        uint8_t status = resp[5];
        // status == 0 means success? 官方：返回1字节为当前命令执行状态（0? 1?），文档里举例中多数为非0为成功，建议直接打印原样
        (void)status;
    }
    return true;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <record_file> <serial_port> [baud=9600] [addr=0x00]\n";
        cerr << "Example: " << argv[0] << " records.bin /dev/ttyUSB0 9600 0x00\n";
        return 1;
    }
    string fname = argv[1];
    string port = argv[2];
    unsigned int baud = 9600;
    if (argc >= 4) baud = (unsigned int)stoi(argv[3]);
    unsigned int addr = 0x00;
    if (argc >= 5) {
        addr = (unsigned int)strtol(argv[4], nullptr, 0);
    }

    // read all records into vector
    ifstream ifs(fname, ios::binary);
    if (!ifs) {
        cerr << "Cannot open file: " << fname << "\n";
        return 2;
    }

    vector<RecordItem> records;
    while (true) {
        RecordItem r;
        ifs.read(reinterpret_cast<char*>(&r), sizeof(r));
        if (!ifs) break;
        // NOTE: this assumes file stored in same endianness as running platform (little-endian typical).
        records.push_back(r);
    }
    if (records.empty()) {
        cerr << "No records found in file.\n";
        return 3;
    }
    cout << "Loaded " << records.size() << " records. Last t = " << records.back().t << " ms\n";

    // open serial
    auto shOpt = openSerial(port, baud);
    if (!shOpt) {
        cerr << "Failed to open serial port " << port << "\n";
        return 4;
    }
    SerialHandle sh = *shOpt;
    cout << "Opened serial port " << port << " at " << baud << " baud. Using device address 0x"
        << hex << addr << dec << "\n";

    uint64_t prev_t = records.front().t;
    // If first record's t is not 0, we will wait relative to that (i.e., first send after (t0 - t0)=0).
    // Iterate all records
    for (size_t i = 0; i < records.size(); ++i) {
        RecordItem& rec = records[i];
        uint64_t wait_ms = 0;
        if (i == 0) wait_ms = 0;
        else {
            // record.t is relative timestamp; compute delta
            if (rec.t >= prev_t) wait_ms = rec.t - prev_t;
            else wait_ms = 0; // non-monotonic: send immediately
        }
        if (wait_ms > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms));
        }
        prev_t = rec.t;

        // build CH9329 keyboard frame: CMD = 0x02, LEN = 8, DATA = {modifier, reserved, keycode[6]}
        uint8_t data8[8];
        data8[0] = rec.modifier;
        data8[1] = rec.reserved; // doc expects this byte to be 0x00, but we forward the recorded reserved
        memcpy(&data8[2], rec.keycode, 6);

        auto frame = buildKbFrame(static_cast<uint8_t>(addr), 0x02, data8, 8);
        bool ok = serialWrite(sh, frame);
        if (!ok) {
            cerr << "Failed to write frame for record " << i << "\n";
            // continue or break? we continue to attempt replay
        }
        else {
            cout << "Sent record " << i << " t=" << rec.t << " ms, modifier=0x"
                << hex << (int)rec.modifier << dec << "\n";
        }

        // read response (chip should respond within 500 ms). Try to read up to 64 bytes.
        auto resp = serialReadWithTimeout(sh, 500, 64);
        if (resp.empty()) {
            cout << "  -> No response (timeout)\n";
        }
        else {
            // simple print
            cout << "  -> Resp (" << resp.size() << " bytes):";
            for (auto b : resp) {
                printf(" %02X", b);
            }
            cout << "\n";
            if (expectAck(resp, 0x02)) {
                cout << "  -> ACK for CMD 0x02\n";
            }
            else {
                cout << "  -> Non-ACK or unexpected response\n";
            }
        }
    }

    closeSerial(sh);
    cout << "Replay finished.\n";
    return 0;
}
