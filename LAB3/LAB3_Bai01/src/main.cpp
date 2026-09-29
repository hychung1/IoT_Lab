#include <Arduino.h>
#include <WiFi.h>

// Khai báo thông tin mạng WiFi
const char* ssid = "HUIT_SV";        // Thay bằng SSID thực tế
const char* password = "";    // Thay bằng Password thực tế

void setup() {
  // Khởi tạo cổng Serial để theo dõi trạng thái
  Serial.begin(115200);
  delay(100);

  Serial.println("\n----------------------------------");
  Serial.println("Bat dau ket noi WiFi...");

  // Cấu hình vi điều khiển ở chế độ Station
  WiFi.mode(WIFI_STA);

  // Bắt đầu kết nối
  WiFi.begin(ssid, password);

  // Chờ kết nối thành công và in ra dấu "." trong lúc chờ
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Thông báo khi kết nối thành công
  Serial.println("\n----------------------------------");
  Serial.println("Ket noi WiFi thanh cong!");
  Serial.print("Dia chi IP nhan duoc: ");
  Serial.println(WiFi.localIP());
  Serial.println("----------------------------------");
}

void loop() {

}
