import psutil
import ipaddress
import requests
import sys

from ipaddr import get_ethernet_ipv6, is_valid_ipv6

# ====== 配置部分 ======
# 预定义的ZoneID映射（格式：{"域名": "ZoneID"}）
ZONES = {
    "yourdomain.top": "yourzoneid1",
    "yourapex.com": "yourzoneid2",
}

# 需要更新的DNS记录列表（格式：[(记录名, 所属域名)]）
DNS_RECORDS_TO_UPDATE = [
    ("domain1.sub.yourdomain.top", "yourdomain.top", False),
    ("domain2.sub.yourdomain.top", "yourdomain.top", False),
    ("yourproxied.yourapex.com", "yourapex.com", True),
]

api_token = '*************************'

def update_cloudflare(ipv6, dns_record_name, zone_name, proxied):
    headers = {
        'Authorization': f'Bearer {api_token}',
        'Content-Type': 'application/json'
    }

    # # 获取Zone ID
    # try:
    #     zones_response = requests.get(
    #         'https://api.cloudflare.com/client/v4/zones',
    #         headers=headers,
    #         params={'name': zone_name}
    #     )
    #     zones_response.raise_for_status()
    #     zones_data = zones_response.json()
    #     if not zones_data['success'] or not zones_data['result']:
    #         print("无法找到Zone，请检查域名或API权限。")
    #         return False
    #     zone_id = zones_data['result'][0]['id']
    # except Exception as e:
    #     print(f"获取Zone ID失败: {e}")
    #     return False

    # 获取并更新DNS记录
    try:
        # 从预定义ZONES获取ZoneID
        zone_id = ZONES.get(zone_name)
        if not zone_id:
            print(f"[错误] 域名 {zone_name} 未配置ZoneID")
            return False
        
        dns_response = requests.get(
            f'https://api.cloudflare.com/client/v4/zones/{zone_id}/dns_records',
            headers=headers,
            params={'name': dns_record_name, 'type': 'AAAA'}
        )
        dns_response.raise_for_status()
        dns_data = dns_response.json()
        records = dns_data['result']

        if not records:
            # 创建新记录
            data = {
                'type': 'AAAA',
                'name': dns_record_name,
                'content': ipv6,
                'ttl': 300,
                'proxied': proxied  # 关键新增字段
            }
            create_response = requests.post(
                f'https://api.cloudflare.com/client/v4/zones/{zone_id}/dns_records',
                headers=headers,
                json=data
            )
            create_response.raise_for_status()
            print("成功创建AAAA记录。")
        else:
            # 更新现有记录
            record = records[0]
            if record['content'] == ipv6:
                print("IPv6地址未变化，无需更新。")
                return True
            data = {
                'type': 'AAAA',
                'name': dns_record_name,
                'content': ipv6,
                'ttl': 300,
                'proxied': proxied  # 关键新增字段
            }
            update_response = requests.put(
                f'https://api.cloudflare.com/client/v4/zones/{zone_id}/dns_records/{record["id"]}',
                headers=headers,
                json=data
            )
            update_response.raise_for_status()
            print("成功更新AAAA记录。")
        return True
    except Exception as e:
        print(f"DNS记录操作失败: {e}")
        return False

# 更新主函数
if __name__ == '__main__':
    current_ip = get_ethernet_ipv6()
    print(current_ip)
    if current_ip and is_valid_ipv6(current_ip):
        print(f"当前IPv6地址: {current_ip}")
        
        # 遍历所有需要更新的记录
        success_count = 0
        for record_config in DNS_RECORDS_TO_UPDATE:
            record_name, zone_name, proxied = record_config  # 解包三元组
            print(f"\n正在处理记录: {record_name} (代理状态: {proxied})")
            if update_cloudflare(current_ip, record_name, zone_name, proxied):
                success_count += 1
        
        print(f"\n操作完成：成功处理 {success_count}/{len(DNS_RECORDS_TO_UPDATE)} 条记录")
        if not success_count == len(DNS_RECORDS_TO_UPDATE):
            sys.exit(1)
    else:
        print("无法获取有效的IPv6地址，请检查网络适配器名称或连接状态。")
        sys.exit(2)

def update_logic():
    pass