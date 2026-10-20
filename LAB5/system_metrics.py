import json
import time
import psutil
import paho.mqtt.client as mqtt

# Cấu hình Broker
BROKER = "127.0.0.1"
PORT = 1883
TOPIC = "gateway/system/metrics"

# Hàm callback khi kết nối thành công
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("[INFO] Đã kết nối thành công tới Local Broker!")
    else:
        print(f"[ERROR] Kết nối thất bại, mã lỗi: {rc}")

# Khởi tạo MQTT Client
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect

# Kết nối tới Local Broker
client.connect(BROKER, PORT, keepalive=60)
client.loop_start()

print(f"[INFO] Bắt đầu đẩy thông số phần cứng lên topic: '{TOPIC}' (Mỗi 5s)...")

try:
    while True:
        # Đọc thông số phần cứng hệ thống
        cpu_usage = psutil.cpu_percent(interval=1)
        ram_usage = psutil.virtual_memory().percent

        # Đóng gói dữ liệu thành định dạng JSON
        payload = {
            "cpu_usage_percent": cpu_usage,
            "ram_usage_percent": ram_usage,
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
        }
        json_payload = json.dumps(payload)

        # Publish dữ liệu lên topic
        result = client.publish(TOPIC, json_payload)

        if result.rc == mqtt.MQTT_ERR_SUCCESS:
            print(f"[SENT] {json_payload}")
        else:
            print(f"[ERROR] Gửi dữ liệu thất bại: {result.rc}")

        # Chờ 5 giây cho lần gửi tiếp theo
        time.sleep(5)

except KeyboardInterrupt:
    print("\n[INFO] Dừng chương trình.")
    client.loop_stop()
    client.disconnect()