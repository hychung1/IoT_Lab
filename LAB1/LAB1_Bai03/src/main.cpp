#include <Arduino.h>

// Khai báo chân kết nối
const int BUTTON_PIN = 18;
const int LED_PIN    = 23;

// Biến lưu trạng thái của LED và nút bấm
bool ledState = LOW;             // Trạng thái hiện tại của LED (LOW = Tắt, HIGH = Sáng)
int lastButtonState = HIGH;      // Trạng thái đọc được từ lần kiểm tra trước (Default HIGH do PULLUP)

// Các biến phục vụ kỹ thuật chống dội (Debounce)
unsigned long lastDebounceTime = 0;  // Mốc thời gian lần cuối trạng thái nút bấm thay đổi
const unsigned long debounceDelay = 50; // Khoảng thời gian lọc nhiễu (50ms)

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Cập nhật trạng thái ban đầu của LED
  digitalWrite(LED_PIN, ledState);
}

void loop() {
  // 1. Đọc tín hiệu hiện tại từ nút bấm
  int reading = digitalRead(BUTTON_PIN);

  // 2. Nếu tín hiệu đọc được thay đổi so với lần đọc trước (do nhiễu hoặc do nhấn nút)
  if (reading != lastButtonState) {
    // Ghi lại mốc thời gian xảy ra sự thay đổi này
    lastDebounceTime = millis();
  }

  // 3. Nếu tín hiệu giữ nguyên trong thời gian dài hơn khoảng debounceDelay (50ms)
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // Xác nhận tín hiệu đã ổn định (không phải do nhiễu rung cơ học)
    // Phát hiện sự kiện "Nhấn xuống" (Edge detection: Tín hiệu chuyển từ HIGH sang LOW)
    static int buttonState = HIGH; // Trạng thái đã xác nhận của nút
    
    if (reading != buttonState) {
      buttonState = reading;

      // Khi nút thực sự được nhấn xuống (LOW)
      if (buttonState == LOW) {
        // Đảo trạng thái LED (Toggle)
        ledState = !ledState;
        digitalWrite(LED_PIN, ledState);
      }
    }
  }

  // Lưu trạng thái tín hiệu đọc được để so sánh ở chu kỳ loop tiếp theo
  lastButtonState = reading;
}