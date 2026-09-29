import random
import time
import requests

# 1. Cấu hình URL Webhook.site của bạn (Thay thế bằng URL thực tế của bạn)
WEBHOOK_URL = "https://webhook.site/c4c48a0a-9466-4c68-8b73-92daeb1b320a"

# Chu kỳ gửi dữ liệu (tính bằng giây)
SEND_INTERVAL = 10


def generate_sensor_data():
    """Hàm sinh dữ liệu cảm biến ngẫu nhiên"""
    return {
        "timestamp": time.strftime("%Y-%m-%d %H:%M:%S"),
        "node_id": "SENSOR_NODE_01",
        "temperature": round(
            random.uniform(20.0, 40.0), 2
        ),  # Nhiệt độ từ 20.0°C - 40.0°C
        "humidity": round(random.uniform(50.0, 90.0), 2),  # Độ ẩm từ 50.0% - 90.0%
    }


def main():
    print("=" * 60)
    print("🚀 BẮT ĐẦU CHƯƠNG TRÌNH GỬI DỮ LIỆU CẢM BIẾN ĐỊNH KỲ TỰ ĐỘNG")
    print(f"⏱️  Chu kỳ gửi: Mỗi {SEND_INTERVAL} giây")
    print("💡 Nhấn Ctrl + C để dừng chương trình an toàn.")
    print("=" * 60)

    count = 1

    # Vòng lặp vô tận mô phỏng thiết bị IoT hoạt động liên tục
    try:
        while True:
            # 1. Sinh dữ liệu
            data = generate_sensor_data()

            print(
                f"\n[Lần {count} - {data['timestamp']}] Đang gửi dữ liệu..."
            )

            # 2. Gửi request kèm xử lý ngoại lệ
            try:
                response = requests.post(WEBHOOK_URL, json=data, timeout=5)

                if response.status_code == 200:
                    print("  ✅ Gửi thành công!")
                    print(
                        f"  📊 Dữ liệu đã gửi: Nhiệt độ = {data['temperature']}°C | Độ ẩm = {data['humidity']}%"
                    )
                else:
                    print(
                        f"  ⚠️ Server phản hồi mã lỗi: {response.status_code}"
                    )

            except requests.exceptions.Timeout:
                print("  ❌ Lỗi: Kết nối quá thời gian (Timeout).")
            except requests.exceptions.ConnectionError:
                print("  ❌ Lỗi: Không thể kết nối tới Server (Kiểm tra mạng).")
            except requests.exceptions.RequestException as e:
                print(f"  ❌ Lỗi không xác định: {e}")

            count += 1

            # 3. Chờ 10 giây trước khi gửi lần tiếp theo
            time.sleep(SEND_INTERVAL)

    except KeyboardInterrupt:
        # Bắt sự kiện khi người dùng nhấn Ctrl + C
        print("\n\n" + "=" * 60)
        print("🛑 Nhận lệnh dừng từ người dùng (Ctrl + C).")
        print("👋 Chương trình đã kết thúc an toàn.")
        print("=" * 60)


if __name__ == "__main__":
    main()