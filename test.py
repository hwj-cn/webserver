
import subprocess
import pymongo
import re

myclient = pymongo.MongoClient("mongodb://localhost:27017")
# myclient = pymongo.MongoClient('localhost',27017)
print(myclient.list_database_names())





PORT = 8080
TEST_PROCTOL = 'http'
TEST_IP = '127.0.0.1'
TEST_ADDR = f'{TEST_PROCTOL}://{TEST_IP}:{PORT}'


def test():
    result = subprocess.run(['bash', './test.sh'], capture_output=True, text=True)

    # 定义匹配 Speed 部分的正则表达式
    speed_pattern = r"Speed=(\d+)\s+pages/min,\s+(\d+)\s+bytes/sec"
    # 定义匹配 Requests 部分的正则表达式
    requests_pattern = r"Requests:\s+(\d+)\s+susceed,\s+(\d+)\s+failed"

    speed_match = re.search(speed_pattern, result.stdout)
    requests_match = re.search(requests_pattern, result.stdout)


    if speed_match:
        pages_per_min, bytes_per_sec = speed_match.groups()
        print("Speed 部分:")
        print("  pages/min:", pages_per_min)
        print("  bytes/sec:", bytes_per_sec)

    if requests_match:
        succeed, failed = requests_match.groups()
        print("Requests 部分:")
        print("  succeed:", succeed)
        print("  failed:", failed)
    
    # print(f'{speed_pages} {speed_bytes} {request_susceed} {request_failed}')
            


    # print(f'std output: {result.stdout}')
    # print(f'std error: {result.stderr}')



test()