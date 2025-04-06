import subprocess
import base58

# 生成256位随机数据
openssl_command = ['openssl', 'rand', '-hex', '256']  # 32字节 = 256位
result = subprocess.run(openssl_command, capture_output=True, text=True)
random_hex = result.stdout.strip()

# 将16进制数据转换为字节
random_bytes = bytes.fromhex(random_hex)

# 进行base58编码
base58_encoded = base58.b58encode(random_bytes).decode('utf-8')

# 截取前72位
truncated_base58 = base58_encoded[:72]

print(f"Your password: {truncated_base58}")

# 使用webdav.exe生成bcrypt值
webdav_command = [
    r"./webdav.exe",
    'bcrypt',
    '--cost', '12',
    truncated_base58
]

result = subprocess.run(webdav_command, capture_output=True, text=True)

# 输出bcrypt值
if result.returncode == 0:
    print(f"Bcrypt Hash: {result.stdout.strip()}")
else:
    print(f"Error generating bcrypt hash: {result.stderr.strip()}")


input('Press Enter to continue... ')