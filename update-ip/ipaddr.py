import psutil
import ipaddress

def get_ethernet_ipv6():
    target_adapter = "以太网"  # 适配器名称（中文Windows中显示的名称）
    ipv6_addresses = []
    
    # 遍历所有网络接口
    for name, addrs in psutil.net_if_addrs().items():
        # 匹配目标适配器（名称包含"以太网"）
        if target_adapter in name:
            for addr in addrs:
                # 筛选IPv6地址，排除临时地址
                if addr.address != "" and addr.family == 23:
                    # 排除临时地址（根据系统特征过滤，例如后缀含"%"的临时标识）
                    if "%" not in addr.address:
                        ipv6_addresses.append(addr.address.split("%")[0])
    
    # 提取第二个有效地址（假设顺序与ipconfig一致）
    if len(ipv6_addresses) >= 2:
        return ipv6_addresses[0] if "::" in ipv6_addresses[1] else ipv6_addresses[1]
    elif len(ipv6_addresses) == 1:
        return ipv6_addresses[0]
    return None

def is_valid_ipv6(address):
    try:
        ipaddress.IPv6Address(address)
        return True
    except ipaddress.AddressValueError:
        return False