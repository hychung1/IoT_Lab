#include <Arduino.h>
#include "DHT.h"

// Định nghĩa chân cắm và loại cảm biến
#define DHTPIN 15        // Chân GPIO nối với chân Data của DHT22
#define DHTTYPE DHT22    // Cảm biến DHT22
#define LED_PIN 23       // Chân GPIO nối với LED cảnh báo

// Khởi tạo đối tượng DHT
DHT dht(DHTPIN, DHTTYPE);

// Ngưỡng nhiệt độ cảnh báo (°C)
const float TEMP_THRESHOLD = 30.0;

void setup() {
  Serial.begin(115200);
  Serial.println("--- CẢNH BÁO NHIỆT ĐỘ QUA NGƯỠNG ---");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Khởi động cảm biến DHT
  dht.begin();
}

void loop() {
  // Đọc giá trị độ ẩm và nhiệt độ từ DHT22
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Kiểm tra xem đọc dữ liệu từ cảm biến có thành công không
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Lỗi: Không thể đọc dữ liệu từ cảm biến DHT22!");
    delay(2000);
    return;
  }

  // In thông tin lên Serial Monitor
  Serial.print("Độ ẩm: ");
  Serial.print(humidity);
  Serial.print("% | Nhiệt độ: ");
  Serial.print(temperature);
  Serial.println("°C");

  // Xử lý logic cảnh báo
  if (temperature > TEMP_THRESHOLD) {
    Serial.println("-> CẢNH BÁO: Nhiệt độ vượt quá 30°C!");
    
    // Nhấp nháy đèn LED để cảnh báo (Chạy trong khoảng thời gian ~1 giây)
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
    digitalWrite(LED_PIN, HIGH);
    delay(250);
    digitalWrite(LED_PIN, LOW);
    delay(250);
  } else {
    // Nếu nhiệt độ <= 30°C, đảm bảo tắt đèn LED
    digitalWrite(LED_PIN, LOW);
    delay(1000); // Tạm dừng 1 giây trước khi đọc chu kỳ tiếp theo
  }
}