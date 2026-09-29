import random
import time
import requests

# 1. URL lấy từ Webhook.site (Thay bằng URL duy nhất của bạn)
WEBHOOK_URL = "https://webhook.site/c4c48a0a-9466-4c68-8b73-92daeb1b320a"

# 2. Chuẩn bị dữ liệu cảm biến
sensor_data = {
    "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime()),
    "node_id": "ESP32_NODE_ROBUST",
    "sensors": {
        "temperature": round(random.uniform(20.0, 35.0), 2),
        "humidity": round(random.uniform(50.0, 90.0), 2),
    },
    "battery_level": 88,
}

# 3. Gửi HTTP POST Request bọc trong khối try...except
try:
    print("Đang gửi dữ liệu cảm biến lên server...")

    # Thêm tham số timeout=3 (chờ tối đa 3 giây)
    response = requests.post(WEBHOOK_URL, json=sensor_data, timeout=3)

    # Kiểm tra nếu HTTP status code là lỗi (4xx hoặc 5xx)
    response.raise_for_status()

    print(
        f"✅ Gửi thành công! Status Code: {response.status_code}"
    )
    print(f"Response: {response.text}")

except requests.exceptions.Timeout:
    # Xử lý khi kết nối bị quá thời gian (Server phản hồi chậm hoặc không phản hồi)
    print("❌ LỖI: Request bị Timeout! Server quá 3 giây không phản hồi.")

except requests.exceptions.ConnectionError:
    # Xử lý khi mất mạng, DNS lỗi, hoặc URL không tồn tại
    print(
        "❌ LỖI: Không thể kết nối tới Server! Vui lòng kiểm tra lại mạng Internet hoặc URL."
    )

except requests.exceptions.HTTPError as http_err:
    # Xử lý khi Server trả về mã lỗi HTTP (ví dụ: 404 Not Found, 500 Internal Server Error)
    print(
        f"❌ LỖI HTTP: Server trả về lỗi - {http_err} (Status Code: {response.status_code})"
    )

except requests.exceptions.RequestException as err:
    # Bắt tất cả các ngoại lệ khác liên quan đến thư viện requests
    print(f"❌ LỖI KHÔNG XÁC ĐỊNH: {err}")