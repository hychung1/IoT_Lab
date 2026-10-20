import paho.mqtt.client as mqtt

BROKER = "127.0.0.1"
PORT = 1883
TOPIC = "gateway/command/#"  # Wildcard # để lắng nghe mọi sub-topic đằng sau gateway/command/

# Callback được gọi khi kết nối thành công đến Broker
def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print(f"[INFO] Đã kết nối thành công tới Broker {BROKER}:{PORT}")
        # Đăng ký subscribe topic gateway/command/#
        client.subscribe(TOPIC)
        print(f"[INFO] Đã subscribe topic: '{TOPIC}'")
        print("[INFO] Đang chờ lệnh điều khiển nội bộ... (Nhấn Ctrl+C để dừng)\n")
    else:
        print(f"[ERROR] Kết nối thất bại, mã lỗi: {rc}")

# Callback được gọi khi nhận được tin nhắn từ Broker
def on_message(client, userdata, msg):
    print("==========================================")
    print(f"[NHẬN LỆNH] Topic  : {msg.topic}")
    print(f"[NHẬN LỆNH] Payload: {msg.payload.decode('utf-8')}")
    print("==========================================")

# Khởi tạo MQTT Client
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

# Kết nối đến Local Broker
client.connect(BROKER, PORT, keepalive=60)

try:
    # Lặp vô hạn để duy trì kết nối và xử lý sự kiện nhận tin nhắn
    client.loop_forever()
except KeyboardInterrupt:
    print("\n[INFO] Dừng chương trình lắng nghe lệnh.")
    client.disconnect()