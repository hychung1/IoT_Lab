#include <Arduino.h>

// Định nghĩa chân cắm LED
#define LED1_PIN 2
#define LED2_PIN 4

// Chu kỳ nhấp nháy (ms)
const unsigned long LED1_INTERVAL = 500;   // LED 1 đảo trạng thái mỗi 500ms
const unsigned long LED2_INTERVAL = 1200;  // LED 2 đảo trạng thái mỗi 1200ms

// Biến lưu mốc thời gian chuyển đổi cuối cùng
unsigned long previousMillis1 = 0;
unsigned long previousMillis2 = 0;

// Biến lưu trạng thái hiện tại của LED (LOW / HIGH)
int ledState1 = LOW;
int ledState2 = LOW;

void setup() {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  digitalWrite(LED1_PIN, ledState1);
  digitalWrite(LED2_PIN, ledState2);
}

void loop() {
  unsigned long currentMillis = millis();

  // ----------------------------------------------------
  // 1. XỬ LÝ NHẤP NHÁY LED 1 (Chu kỳ 500ms)
  // ----------------------------------------------------
  if (currentMillis - previousMillis1 >= LED1_INTERVAL) {
    previousMillis1 = currentMillis; // Lưu lại mốc thời gian vừa đổi trạng thái

    // Đảo trạng thái LED 1
    ledState1 = (ledState1 == LOW) ? HIGH : LOW;
    digitalWrite(LED1_PIN, ledState1);
  }

  // ----------------------------------------------------
  // 2. XỬ LÝ NHẤP NHÁY LED 2 (Chu kỳ 1200ms)
  // ----------------------------------------------------
  if (currentMillis - previousMillis2 >= LED2_INTERVAL) {
    previousMillis2 = currentMillis; // Lưu lại mốc thời gian vừa đổi trạng thái

    // Đảo trạng thái LED 2
    ledState2 = (ledState2 == LOW) ? HIGH : LOW;
    digitalWrite(LED2_PIN, ledState2);
  }
}