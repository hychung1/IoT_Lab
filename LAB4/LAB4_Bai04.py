import random
import time
import requests

# 1. URL lấy từ Webhook.site (Thay bằng URL duy nhất của bạn)
WEBHOOK_URL = "https://webhook.site/c4c48a0a-9466-4c68-8b73-92daeb1b320a"

# 2. Định nghĩa các Headers xác thực (Authentication Headers)
headers = {
    "X-API-Key": "my_secret_key_123",
    "Authorization": "Bearer token_xyz",
    "Content-Type": "application/json",  # Khai báo định dạng JSON
}

# 3. Dữ liệu cảm biến cần gửi
sensor_data = {
    "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
    "node_id": "ESP32_NODE_SECURE",
    "sensors": {
        "temperature": round(random.uniform(22.0, 30.0), 2),
        "humidity": round(random.uniform(60.0, 80.0), 2),
    },
    "battery_level": 95,
}

# 4. Gửi HTTP POST Request kèm theo tham số headers
print("Đang gửi dữ liệu cảm biến có xác thực lên server...")
response = requests.post(WEBHOOK_URL, json=sensor_data, headers=headers)

# 5. In kết quả phản hồi từ Server
print(f"Status Code: {response.status_code}")
print(f"Response Text: {response.text}")