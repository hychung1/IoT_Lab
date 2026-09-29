#include <Arduino.h>
#include <WiFi.h>

const char* ssid = "HUIT_SV";
const char* password = "";

// Hàm xử lý sự kiện WiFi
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("[Event] WiFi Station khoi tao.");
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.println("\n[Event] Ket noi thanh cong!");
      Serial.print("[Event] IP Address: ");
      Serial.println(WiFi.localIP());
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("\n[Event] Mat ket noi WiFi! Dang tu dong thu lai...");
      // Thử kết nối lại ngay khi ngắt kết nối
      WiFi.reconnect();
      break;

    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Đăng ký Callback lắng nghe sự kiện
  WiFi.onEvent(WiFiEvent);

  // Đặt chế độ Station và bắt đầu kết nối
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
}

void loop() {
  // Hàm loop hoàn toàn trống, không cần tốn tài nguyên kiểm tra trạng thái
  delay(1000);
}