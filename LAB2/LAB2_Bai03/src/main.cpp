#include <Arduino.h>

// Định nghĩa chân LED tích hợp (Onboard LED của ESP32 là GPIO 2)
#define LED_HEARTBEAT_PIN 2

// Khai báo khoảng thời gian cho các tiến trình (ms)
const unsigned long LED_INTERVAL = 500;     // Đảo trạng thái LED mỗi 500ms (Chu kỳ chớp 1s)
const unsigned long SERIAL_INTERVAL = 3000; // Gửi dữ liệu Serial mỗi 3000ms (3s)

// Biến lưu mốc thời gian trước đó cho từng tiến trình
unsigned long previousMillisLED = 0;
unsigned long previousMillisSerial = 0;

// Biến lưu trạng thái LED
int ledState = LOW;

void setup() {
  // Khởi tạo giao tiếp Serial với Baudrate 115200
  Serial.begin(115200);

  // Cấu hình chân LED là OUTPUT
  pinMode(LED_HEARTBEAT_PIN, OUTPUT);
  digitalWrite(LED_HEARTBEAT_PIN, ledState);

  Serial.println("=== HELLO! HEPTHTONG BAT DAU HOAT DONG ===");
}

void loop() {
  unsigned long currentMillis = millis();

  // ----------------------------------------------------
  // 1. TIẾN TRÌNH LED HEARTBEAT (Chu kỳ nhấp nháy 1s)
  // ----------------------------------------------------
  if (currentMillis - previousMillisLED >= LED_INTERVAL) {
    previousMillisLED = currentMillis; // Cập nhật lại mốc thời gian

    // Đảo trạng thái LED
    ledState = (ledState == LOW) ? HIGH : LOW;
    digitalWrite(LED_HEARTBEAT_PIN, ledState);
  }

  // ----------------------------------------------------
  // 2. TIẾN TRÌNH GỬI DỮ LIỆU SERIAL (Định kỳ 3s)
  // ----------------------------------------------------
  if (currentMillis - previousMillisSerial >= SERIAL_INTERVAL) {
    previousMillisSerial = currentMillis; // Cập nhật lại mốc thời gian

    // Tính thời gian hoạt động tính theo giây và miligiây
    unsigned long totalSeconds = currentMillis / 1000;

    // Gửi thông báo kèm thời gian uptime
    Serial.print("[SYSTEM RUNNING] Uptime: ");
    Serial.print(totalSeconds);
    Serial.print("s (");
    Serial.print(currentMillis);
    Serial.println(" ms)");
  }
}