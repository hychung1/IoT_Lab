#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

// --- Cấu hình Cảm biến DHT11 ---
#define DHTPIN 15       // Chân GPIO kết nối dữ liệu DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Cấu hình Wi-Fi ---
// Trên Wokwi, tên Wi-Fi cố định là "Wokwi-GUEST" và không có mật khẩu ""
const char* ssid = "ESP32 DEV KIT";
const char* password = "123";

// --- Cấu hình URL Server ---
// Thay thế bằng Webhook.site URL của bạn (hoặc IP Local Server nếu chạy mạch thật cùng LAN)
const char* serverName = "http://127.0.0.1:5000";

void setup() {
  Serial.begin(115200);
  
  // Khởi tạo cảm biến DHT
  dht.begin();

  // Kết nối Wi-Fi
  Serial.print("Đang kết nối Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ Wi-Fi đã kết nối thành công!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Đọc dữ liệu từ cảm biến DHT11
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Kiểm tra nếu cảm biến bị lỗi đọc
  if (isnan(temp) || isnan(hum)) {
    Serial.println("❌ Lỗi: Không thể đọc dữ liệu từ cảm biến DHT11!");
    delay(5000);
    return;
  }

  // Kiểm tra kết nối Wi-Fi trước khi gửi request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Khởi tạo kết nối HTTP
    http.begin(serverName);

    // Bổ sung Header định dạng dữ liệu JSON
    http.addHeader("Content-Type", "application/json");

    // Đóng gói dữ liệu JSON sử dụng thư viện ArduinoJson
    StaticJsonDocument<200> doc;
    doc["node_id"] = "ESP32_WOKWI_NODE";
    doc["temperature"] = temp;
    doc["humidity"] = hum;
    doc["battery_level"] = 100;

    String jsonString;
    serializeJson(doc, jsonString);

    Serial.println("\n------------------------------------");
    Serial.print("Đang gửi HTTP POST request đến: ");
    Serial.println(serverName);
    Serial.print("Dữ liệu JSON: ");
    Serial.println(jsonString);

    // Gửi POST request
    int httpResponseCode = http.POST(jsonString);

    // Kiểm tra mã phản hồi trả về
    if (httpResponseCode > 0) {
      Serial.print("✅ Gửi thành công! Status Code: ");
      Serial.println(httpResponseCode);
      String response = http.getString();
      Serial.print("Server Response: ");
      Serial.println(response);
    } else {
      Serial.print("❌ Gửi thất bại! Error Code: ");
      Serial.println(httpResponseCode);
    }

    // Giải phóng tài nguyên HTTP
    http.end();
  } else {
    Serial.println("❌ Wi-Fi bị ngắt kết nối!");
  }

  // Chờ 10 giây trước khi gửi lần tiếp theo
  delay(10000);
}