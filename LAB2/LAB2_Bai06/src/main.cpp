#include <Arduino.h>

// Định nghĩa chân cắm đèn LED giao thông
#define LED_RED_PIN 27
#define LED_YELLOW_PIN 26
#define LED_GREEN_PIN 25

// Định nghĩa các trạng thái của đèn giao thông
enum TrafficState {
  STATE_RED,
  STATE_GREEN,
  STATE_YELLOW
};

// Cấu hình thời gian duy trì cho từng trạng thái (ms)
const unsigned long DURATION_RED = 5000;    // Đỏ: 5 giây
const unsigned long DURATION_GREEN = 4000;  // Xanh: 4 giây
const unsigned long DURATION_YELLOW = 2000; // Vàng: 2 giây

// Biến quản lý trạng thái và thời gian
TrafficState currentState = STATE_RED;
unsigned long stateStartTime = 0;

// Khai báo prototype hàm
void setTrafficLeds(bool red, bool yellow, bool green);
void printStateInfo(const char* stateName, unsigned long durationMs);

void setup() {
  Serial.begin(115200);

  pinMode(LED_RED_PIN, OUTPUT);
  pinMode(LED_YELLOW_PIN, OUTPUT);
  pinMode(LED_GREEN_PIN, OUTPUT);

  // Bắt đầu ở trạng thái ĐÈN ĐỎ
  currentState = STATE_RED;
  stateStartTime = millis();
  setTrafficLeds(true, false, false);
  printStateInfo("RED", DURATION_RED);
}

void loop() {
  unsigned long currentMillis = millis();

  // Kiểm tra điều kiện chuyển trạng thái dựa theo thời gian đã trôi qua
  switch (currentState) {
    case STATE_RED:
      if (currentMillis - stateStartTime >= DURATION_RED) {
        currentState = STATE_GREEN;       // Chuyển sang Xanh
        stateStartTime = currentMillis;   // Cập nhật lại mốc thời gian
        setTrafficLeds(false, false, true);
        printStateInfo("GREEN", DURATION_GREEN);
      }
      break;

    case STATE_GREEN:
      if (currentMillis - stateStartTime >= DURATION_GREEN) {
        currentState = STATE_YELLOW;      // Chuyển sang Vàng
        stateStartTime = currentMillis;   // Cập nhật lại mốc thời gian
        setTrafficLeds(false, true, false);
        printStateInfo("YELLOW", DURATION_YELLOW);
      }
      break;

    case STATE_YELLOW:
      if (currentMillis - stateStartTime >= DURATION_YELLOW) {
        currentState = STATE_RED;         // Quay lại Đỏ
        stateStartTime = currentMillis;   // Cập nhật lại mốc thời gian
        setTrafficLeds(true, false, false);
        printStateInfo("RED", DURATION_RED);
      }
      break;
  }
}

// Hàm bật/tắt đồng thời 3 LED
void setTrafficLeds(bool red, bool yellow, bool green) {
  digitalWrite(LED_RED_PIN, red ? HIGH : LOW);
  digitalWrite(LED_YELLOW_PIN, yellow ? HIGH : LOW);
  digitalWrite(LED_GREEN_PIN, green ? HIGH : LOW);
}

// Hàm in trạng thái ra Serial Monitor
void printStateInfo(const char* stateName, unsigned long durationMs) {
  Serial.print("[TRAFFIC LIGHT] Trang thai: ");
  Serial.print(stateName);
  Serial.print(" | Thoi gian: ");
  Serial.print(durationMs / 1000);
  Serial.println("s");
}