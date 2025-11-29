import serial
import time

# CH9329 键盘指令常量
CMD_KB = 0x02
ADDR = 0x00

# 记录的 10 条数据（时间戳与 keycode）
records = [
    (2501, [31, 0, 0, 0, 0, 0]),
    (2677, [0,  0, 0, 0, 0, 0]),
    (3149, [32, 0, 0, 0, 0, 0]),
    (3269, [0,  0, 0, 0, 0, 0]),
    (3469, [32, 0, 0, 0, 0, 0]),
    (3533, [0,  0, 0, 0, 0, 0]),
    (3637, [32, 0, 0, 0, 0, 0]),
    (3742, [0,  0, 0, 0, 0, 0]),
    (3813, [32, 0, 0, 0, 0, 0]),
    (3917, [0,  0, 0, 0, 0, 0]),
]


def build_kb_frame(addr, modifier, keycodes):
    # CH9329 frame:
    # 0: 0x57
    # 1: 0xAB
    # 2: addr
    # 3: cmd = 0x02 (keyboard)
    # 4: len = 8
    # 5: modifier
    # 6: reserved (0)
    # 7..12: 6 bytes keycode
    # 13: checksum = sum(frame[0:13]) % 256

    frame = [
        0x57, 0xAB, addr, CMD_KB, 8,
        modifier, 0
    ] + keycodes

    checksum = sum(frame) & 0xFF
    frame.append(checksum)
    return bytes(frame)


def main():
    ser = serial.Serial("COM4", 9600, timeout=0.2)

    prev_t = records[0][0]

    for (t, keycodes) in records:
        # wait
        wait_ms = max(0, t - prev_t)
        time.sleep(wait_ms / 1000.0)
        prev_t = t

        # modifier = 0
        modifier = 0

        frame = build_kb_frame(ADDR, modifier, keycodes)
        print(f"Sending: {frame.hex(' ')}")

        ser.write(frame)
        resp = ser.read(64)
        if resp:
            print("Resp:", resp.hex(" "))
        else:
            print("Resp: <timeout>")

    ser.close()
    print("Done.")


if __name__ == "__main__":
    main()
