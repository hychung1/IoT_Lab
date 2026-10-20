import json
import time
import paho.mqtt.client as mqtt

BROKER = "127.0.0.1"
PORT = 1883
SUB_TOPIC = "sensor/+/data"
PUB_TOPIC = "gateway/aggregated_report"

# Bộ nhớ tạm lưu trữ dữ liệu từ các sensor node
sensor_data_store = {}

def on_connect(client, userdata, flags, rc, properties=None):
    if rc == 0:
        print(f"[INFO] Đã kết nối Broker. Subscribe topic: {SUB_TOPIC}")
        client.subscribe(SUB_TOPIC)
    else:
        print(f"[ERROR] Kết nối thất bại, mã lỗi: {rc}")

def on_message(client, userdata, msg):
    global sensor_data_store
    try:
        # Lấy node_id từ topic (ví dụ topic: sensor/node1/data -> node_id là 'node1')
        topic_parts = msg.topic.split('/')
        node_id = topic_parts[1]

        # Giải mã nội dung tin nhắn
        payload_str = msg.payload.decode('utf-8')
        try:
            data = json.loads(payload_str)
        except json.JSONDecodeError:
            data = payload_str

        # Cập nhật dữ liệu mới nhất của node vào bộ nhớ tạm
        sensor_data_store[node_id] = {
            "data": data,
            "last_updated": time.strftime("%Y-%m-%d %H:%M:%S")
        }
        print(f"[REC] Nhận dữ liệu từ '{node_id}': {data}")
    except Exception as e:
        print(f"[ERROR] Lỗi xử lý tin nhắn: {e}")

client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.on_connect = on_connect
client.on_message = on_message

client.connect(BROKER, PORT, keepalive=60)
client.loop_start()

print("[INFO] Data Aggregator đang chạy. Gom dữ liệu và Publish mỗi 10 giây...\n")

try:
    while True:
        time.sleep(10)

        if sensor_data_store:
            # Đóng gói dữ liệu tổng hợp
            aggregated_payload = {
                "gateway_timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
                "total_nodes": len(sensor_data_store),
                "sensors": sensor_data_store.copy()
            }
            json_payload = json.dumps(aggregated_payload, ensure_ascii=False)

            # Publish gói dữ liệu tổng hợp
            client.publish(PUB_TOPIC, json_payload)
            print(f"\n[AGGREGATED REPORT SENT] Topic: {PUB_TOPIC}")
            print(f"Payload: {json_payload}\n")
        else:
            print("[INFO] Chưa có dữ liệu từ sensor node nào trong 10s qua.")

except KeyboardInterrupt:
    print("\n[INFO] Dừng chương trình Aggregator.")
    client.loop_stop()
    client.disconnect()