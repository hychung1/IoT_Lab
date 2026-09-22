#include <Arduino.h>

// Định nghĩa chân cắm
#define BUTTON_PIN 18       // Nút nhấn chuyển mức độ sáng
#define PWM_LED_PIN 4       // LED điều khiển độ sáng PWM
#define HEARTBEAT_LED_PIN 2 // LED báo nguồn nhấp nháy 1.5s

// Cấu hình PWM cho ESP32 Core v2.x
#define PWM_CHANNEL 0       // Khai báo kênh PWM (ESP32 hỗ trợ kênh từ 0 -> 15)
#define PWM_FREQ 5000       // Tần số PWM 5kHz
#define PWM_RESOLUTION 8    // Độ phân giải 8-bit (Giá trị từ 0 -> 255)

// Các mức độ sáng (%) tương ứng: 0%, 25%, 50%, 75%, 100%
const int BRIGHTNESS_LEVELS[] = {0, 25, 50, 75, 100};
const int NUM_LEVELS = sizeof(BRIGHTNESS_LEVELS) / sizeof(BRIGHTNESS_LEVELS[0]);
int currentLevelIndex = 0; // Mặc định ở mức 0%

// ----------------------------------------------------
// BIẾN CHO TÁC VỤ 1: ĐÈN BÁO NGUỒN (Chu kỳ 1.5 giây = 1500ms)
// ----------------------------------------------------
unsigned long lastHeartbeatTime = 0;
const unsigned long HEARTBEAT_INTERVAL = 750; // Sáng 750ms, Tắt 750ms
int heartbeatState = LOW;

// ----------------------------------------------------
// BIẾN CHO TÁC VỤ 2: XỬ LÝ NÚT BẤM (DEBOUNCE)
// ----------------------------------------------------
int lastButtonReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 50;

// Khai báo prototype các hàm tác vụ
void taskHeartbeatLED(unsigned long currentMillis);
void taskButtonPWM(unsigned long currentMillis);
void updatePWMDutyCycle();

void setup() {
  Serial.begin(115200);

  // Cấu hình chân GPIO
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(HEARTBEAT_LED_PIN, OUTPUT);
  digitalWrite(HEARTBEAT_LED_PIN, heartbeatState);

  // === CẤU HÌNH PWM TƯƠNG THÍCH CORE V2.X ===
  // 1. Cấu hình tần số và độ phân giải cho kênh PWM_CHANNEL
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  
  // 2. Gán chân GPIO4 vào kênh PWM_CHANNEL vừa cấu hình
  ledcAttachPin(PWM_LED_PIN, PWM_CHANNEL);

  // Cập nhật mức sáng ban đầu (0%)
  updatePWMDutyCycle();

  Serial.println("=== DE DIEU KHIEN DO SANG LED BANG NUT BAM (PWM) ===");
}

void loop() {
  unsigned long currentMillis = millis();

  // Thực thi 2 tác vụ song song
  taskHeartbeatLED(currentMillis);
  taskButtonPWM(currentMillis);
}

// ----------------------------------------------------
// TÁC VỤ 1: Nhấp nháy LED báo nguồn (Chu kỳ 1.5 giây)
// ----------------------------------------------------
void taskHeartbeatLED(unsigned long currentMillis) {
  if (currentMillis - lastHeartbeatTime >= HEARTBEAT_INTERVAL) {
    lastHeartbeatTime = currentMillis;

    heartbeatState = (heartbeatState == LOW) ? HIGH : LOW;
    digitalWrite(HEARTBEAT_LED_PIN, heartbeatState);
  }
}

// ----------------------------------------------------
// TÁC VỤ 2: Đọc nút bấm chuyển đổi mức sáng PWM
// ----------------------------------------------------
void taskButtonPWM(unsigned long currentMillis) {
  int currentReading = digitalRead(BUTTON_PIN);

  if (currentReading != lastButtonReading) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > DEBOUNCE_DELAY) {
    if (currentReading != stableButtonState) {
      stableButtonState = currentReading;

      // Nhận diện sự kiện BẤM NÚT (HIGH -> LOW)
      if (stableButtonState == LOW) {
        currentLevelIndex = (currentLevelIndex + 1) % NUM_LEVELS;
        updatePWMDutyCycle();
      }
    }
  }

  lastButtonReading = currentReading;
}

// ----------------------------------------------------
// Hàm tính toán và xuất xung PWM ra kênh PWM_CHANNEL
// ----------------------------------------------------
void updatePWMDutyCycle() {
  int percent = BRIGHTNESS_LEVELS[currentLevelIndex];
  
  // Quy đổi % (0 - 100%) sang giá trị Duty Cycle 8-bit (0 - 255)
  int dutyCycle = map(percent, 0, 100, 0, 255);

  // Xuất giá trị PWM vào KÊNH (PWM_CHANNEL)
  ledcWrite(PWM_CHANNEL, dutyCycle);

  Serial.print("-> [PWM LED] Muc do sang: ");
  Serial.print(percent);
  Serial.print("% (Duty Cycle: ");
  Serial.print(dutyCycle);
  Serial.println("/255)");
}