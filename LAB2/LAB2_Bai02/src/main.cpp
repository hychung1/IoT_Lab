#include <Arduino.h>

// Định nghĩa chân cắm
#define BUTTON_PIN 18
#define LED1_PIN 4
#define LED2_PIN 5

// Biến quản lý trạng thái LED
bool ledState = false; // false = TẮT, true = BẬT

// Biến cho thuật toán Debounce bằng millis()
int lastButtonReading = HIGH;          // Giá trị đọc thô trước đó từ nút bấm
int stableButtonState = HIGH;          // Trạng thái nút bấm đã qua lọc nhiễu
unsigned long lastDebounceTime = 0;    // Mốc thời gian thay đổi trạng thái thô gần nhất
const unsigned long debounceDelay = 50; // Thời gian chờ lọc nhiễu (50ms)

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  // Đặt trạng thái ban đầu cho 2 LED
  digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
  digitalWrite(LED2_PIN, ledState ? HIGH : LOW);
}

void loop() {
  unsigned long currentMillis = millis();
  int currentReading = digitalRead(BUTTON_PIN);

  // 1. Nếu giá trị tín hiệu thô thay đổi (do nhấn hoặc nhiễu dội)
  if (currentReading != lastButtonReading) {
    lastDebounceTime = currentMillis; // Reset lại bộ đếm thời gian
  }

  // 2. Nếu tín hiệu giữ ổn định đủ lâu (hơn 50ms)
  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    // Nếu trạng thái ổn định mới khác với trạng thái nút bấm ghi nhận trước đó
    if (currentReading != stableButtonState) {
      stableButtonState = currentReading;

      // Nhận diện sự kiện BẤM NÚT (Sườn xuống: HIGH -> LOW)
      if (stableButtonState == LOW) {
        ledState = !ledState; // Đảo trạng thái 2 LED

        digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
        digitalWrite(LED2_PIN, ledState ? HIGH : LOW);
      }
    }
  }

  // Cập nhật giá trị đọc thô cho lần lặp sau
  lastButtonReading = currentReading;
}