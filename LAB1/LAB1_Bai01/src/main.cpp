#include <Arduino.h>

// Khai báo các chân GPIO kết nối với LED
const int LED_GREEN  = 25;
const int LED_YELLOW = 26;
const int LED_RED    = 27;

void setup() {
  // Cấu hình các chân điều khiển LED là OUTPUT
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  // Tắt tất cả các đèn khi bắt đầu
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
}

void loop() {
  // 1. Đèn XANH bật 3 giây
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  delay(3000); // 3000ms = 3 giây

  // 2. Đèn VÀNG bật 1 giây
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, HIGH);
  digitalWrite(LED_RED, LOW);
  delay(1000); // 1000ms = 1 giây

  // 3. Đèn ĐỎ bật 3 giây
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, HIGH);
  delay(3000); // 3000ms = 3 giây
}