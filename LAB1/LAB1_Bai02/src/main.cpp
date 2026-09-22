#include <Arduino.h>

// Khai báo chân GPIO
const int BUTTON_PIN = 18;
const int LED_PIN    = 23;

void setup() {
  // Cấu hình chân điều khiển LED là OUTPUT
  pinMode(LED_PIN, OUTPUT);
  
  // Cấu hình chân nút bấm sử dụng điện trở kéo lên nội (INPUT_PULLUP)
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Đọc trạng thái nút bấm
  int buttonState = digitalRead(BUTTON_PIN);

  // Vì dùng INPUT_PULLUP:
  // - Khi nhấn nút: chân nối đất -> LOW (0)
  // - Khi nhả nút: nhờ điện trở kéo lên -> HIGH (1)
  if (buttonState == LOW) {
    digitalWrite(LED_PIN, HIGH); // Nhấn giữ nút -> Đèn sáng
  } else {
    digitalWrite(LED_PIN, LOW);  // Nhả nút -> Đèn tắt
  }
}