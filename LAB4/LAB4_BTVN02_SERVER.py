from flask import Flask, jsonify, request

# Khởi tạo ứng dụng Flask
app = Flask(__name__)


# Định nghĩa Route /api/sensor nhận phương thức POST
@app.route("/api/sensor", methods=["POST"])
def handle_sensor_data():
    # 1. Kiểm tra xem request có chứa dữ liệu dạng JSON không
    if not request.is_json:
        return (
            jsonify(
                {"status": "error", "message": "Request must be JSON"}
            ),
            400,
        )

    # 2. Đọc dữ liệu JSON gửi lên từ Client
    data = request.get_json()

    # 3. In dữ liệu nhận được ra màn hình Console của Server
    print("\n" + "=" * 50)
    print("📩 [SERVER] NHẬN ĐƯỢC REQUEST MỚI!")
    print(f"  📍 IP Client: {request.remote_addr}")
    print(f"  📦 Dữ liệu JSON: {data}")
    print("=" * 50)

    # 4. Trả về phản hồi JSON với Status Code 200 OK
    response_body = {
        "status": "success",
        "message": "Data received successfully",
    }
    return jsonify(response_body), 200


if __name__ == "__main__":
    print("🚀 Local Server đang chạy tại http://127.0.0.1:5000")
    print("📌 Endpoint lắng nghe: http://127.0.0.1:5000/api/sensor")
    # Chạy server ở máy cục bộ port 5000
    app.run(host="127.0.0.1", port=5000, debug=True)