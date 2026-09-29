import random
import time
import requests

# Endpoint của Local Flask Server
LOCAL_URL = "http://127.0.0.1:5000/api/sensor"

# Dữ liệu cảm biến cần gửi
sensor_data = {
    "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
    "node_id": "ESP32_LOCAL_NODE",
    "temperature": round(random.uniform(22.0, 36.0), 2),
    "humidity": round(random.uniform(55.0, 85.0), 2),
    "battery_level": 90,
}

try:
    print(f"🚀 Đang gửi dữ liệu tới {LOCAL_URL}...")

    # Gửi POST request tới Local Server
    response = requests.post(LOCAL_URL, json=sensor_data, timeout=5)

    # In thông tin phản hồi từ Flask Server
    print(f"✅ Mã trạng thái (Status Code): {response.status_code}")
    print(f"📩 Phản hồi từ Server: {response.json()}")

except requests.exceptions.ConnectionError:
    print(
        "❌ LỖI: Không thể kết nối tới Local Server! Vui lòng kiểm tra lại xem app_server.py đã chạy chưa."
    )
except requests.exceptions.RequestException as e:
    print(f"❌ LỖI KHÔNG XÁC ĐỊNH: {e}")