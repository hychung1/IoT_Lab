import random
import time
import requests

# 1. URL lấy từ Webhook.site (Thay thế bằng URL duy nhất của bạn)
WEBHOOK_URL = "https://webhook.site/c4c48a0a-9466-4c68-8b73-92daeb1b320a"

# 2. Tạo dữ liệu cảm biến (sử dụng Python Dictionary)
sensor_data = {
    "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
    "node_id": "ESP32_NODE_01",
    "sensors": {
        "temperature": round(random.uniform(20.0, 35.0), 2),  # Nhiệt độ ngẫu nhiên từ 20-35°C
        "humidity": round(random.uniform(50.0, 90.0), 2),  # Độ ẩm ngẫu nhiên từ 50-90%
        "light_intensity": random.randint(100, 800),  # Cường độ ánh sáng (lux)
    },
    "battery_level": random.randint(80, 100),  # Phần trăm pin
}

# 3. Gửi HTTP POST Request
# Tham số json=sensor_data sẽ tự động:
#   - Chuyển đổi Python dict sang chuỗi JSON chuẩn
#   - Thêm Header 'Content-Type: application/json'
print("Đang gửi dữ liệu cảm biến lên server...")
response = requests.post(WEBHOOK_URL, json=sensor_data)

# 4. In kết quả phản hồi từ Server
print(f"Status Code: {response.status_code}")
print(f"Response Text: {response.text}")