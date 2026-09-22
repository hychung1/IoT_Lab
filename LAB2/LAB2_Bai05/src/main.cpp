#include <Arduino.h>

// Định nghĩa chân cắm hardware
#define LED1_PIN 2      // LED nhấp nháy định kỳ 300ms
#define LED2_PIN 4      // LED điều khiển qua nút nhấn
#define BUTTON_PIN 18   // Nút nhấn chuyển trạng thái LED2

// ----------------------------------------------------
// BIẾN TOÀN CỤC CHO CÁC TÁC VỤ
// ----------------------------------------------------
// Tác vụ 1 (Time-triggered): Blink LED1
unsigned long lastBlinkTime = 0;
const unsigned long BLINK_INTERVAL = 300; // 300ms
int led1State = LOW;

// Tác vụ 2 (Event-driven): Button & LED2
bool led2State = false;
int lastButtonReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50; // 50ms

// Tác vụ 3 (Time-triggered & Benchmark): Đo tần số loop()
unsigned long lastBenchmarkTime = 0;
const unsigned long BENCHMARK_INTERVAL = 1000; // 1000ms (1s)
unsigned long loopCounter = 0; // Đếm số vòng lặp loop()

// === KHAI BÁO PROTOTYPE HÀM TÁC VỤ ===
void taskBlinkLED1(unsigned long currentMillis);
void taskButtonLED2(unsigned long currentMillis);
void taskBenchmarkLoop(unsigned long currentMillis);

void setup() {
  Serial.begin(115200);

  // Cấu hình GPIO
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED1_PIN, led1State);
  digitalWrite(LED2_PIN, led2State ? HIGH : LOW);

  Serial.println("=== BAT DAU MO HINH MULTI-TASKING ===");
}

void loop() {
  unsigned long currentMillis = millis();

  // Đếm số lần hàm loop() thực thi
  loopCounter++;

  // Thực thi 3 tác vụ song song độc lập
  taskBlinkLED1(currentMillis);
  taskButtonLED2(currentMillis);
  taskBenchmarkLoop(currentMillis);
}

// ----------------------------------------------------
// TÁC VỤ 1: Nhấp nháy LED 1 mỗi 300ms (Time-triggered)
// ----------------------------------------------------
void taskBlinkLED1(unsigned long currentMillis) {
  if (currentMillis - lastBlinkTime >= BLINK_INTERVAL) {
    lastBlinkTime = currentMillis;

    led1State = (led1State == LOW) ? HIGH : LOW;
    digitalWrite(LED1_PIN, led1State);
  }
}

// ----------------------------------------------------
// TÁC VỤ 2: Đọc nút nhấn đảo trạng thái LED 2 (Event-driven)
// ----------------------------------------------------
void taskButtonLED2(unsigned long currentMillis) {
  int currentReading = digitalRead(BUTTON_PIN);

  if (currentReading != lastButtonReading) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (currentReading != stableButtonState) {
      stableButtonState = currentReading;

      // Nhận diện sự kiện BẤM NÚT (HIGH -> LOW)
      if (stableButtonState == LOW) {
        led2State = !led2State;
        digitalWrite(LED2_PIN, led2State ? HIGH : LOW);
        Serial.print("-> [EVENT] Nut nhan: LED 2 -> ");
        Serial.println(led2State ? "ON" : "OFF");
      }
    }
  }

  lastButtonReading = currentReading;
}

// ----------------------------------------------------
// TÁC VỤ 3: Benchmark tần số loop() trong 1s (Time-triggered)
// ----------------------------------------------------
void taskBenchmarkLoop(unsigned long currentMillis) {
  if (currentMillis - lastBenchmarkTime >= BENCHMARK_INTERVAL) {
    lastBenchmarkTime = currentMillis;

    Serial.print("[BENCHMARK] So vong lap loop() trong 1 giay: ");
    Serial.println(loopCounter);

    // Reset lại bộ đếm cho giây tiếp theo
    loopCounter = 0;
  }
}