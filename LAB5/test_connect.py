import time
import paho.mqtt.client as mqtt

# Hàm callback được gọi khi Client kết nối thành công/thất bại tới Broker
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print("==========================================")
        print("[SUCCESS] Kết nối đến Local MQTT Broker thành công!")
        print("==========================================")
    else:
        print(f"[ERROR] Kết nối thất bại, Mã lỗi (rc): {rc}")

# Khởi tạo MQTT Client
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)

# Gán hàm callback khi kết nối
client.on_connect = on_connect

# Thiết lập kết nối tới 127.0.0.1 (localhost) ở cổng 1883
broker_address = "127.0.0.1"
port = 1883

print(f"Đang kết nối tới Broker tại {broker_address}:{port}...")
client.connect(broker_address, port=port, keepalive=60)

# Bắt đầu vòng lặp xử lý mạng trong background
client.loop_start()

# Chờ 3 giây để nhận phản hồi kết nối từ Broker
time.sleep(3)

# Ngắt kết nối và dừng vòng lặp
client.loop_stop()
client.disconnect()