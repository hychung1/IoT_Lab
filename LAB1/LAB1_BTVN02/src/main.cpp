#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Định nghĩa chân nút bấm
#define BUTTON_A_PIN 18 // Nút Start / Pause
#define BUTTON_B_PIN 19 // Nút Reset

LiquidCrystal_I2C lcd(0x27, 16, 2);

// Khai báo kiểu trạng thái Stopwatch dùng enum
enum State {
  STOPPED,
  RUNNING
};

State currentState = STOPPED;

// Biến quản lý thời gian (ms)
unsigned long elapsedTime = 0;   // Tổng thời gian tích lũy
unsigned long startTime = 0;     // Mốc thời gian bắt đầu chạy lượt hiện tại

// Khai báo biến lọc nhiễu dội (Debounce) cho 2 nút
int lastStateA = HIGH;
int lastStateB = HIGH;
unsigned long lastDebounceTimeA = 0;
unsigned long lastDebounceTimeB = 0;
const unsigned long debounceDelay = 50;

// === KHAI BÁO PROTOTYPE HÀM (Thêm dòng này để sửa lỗi) ===
void updateDisplay(unsigned long totalMs);

void setup() {
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  // In giao diện cố định
  lcd.setCursor(0, 0);
  lcd.print("--- STOPWATCH --");
  updateDisplay(0); // Hiển thị 00:00:000 ban đầu
}

void loop() {
  unsigned long currentMillis = millis();

  // --------------------------------------------------
  // 1. XỬ LÝ NÚT A: START / PAUSE
  // --------------------------------------------------
  int readingA = digitalRead(BUTTON_A_PIN);
  if (readingA != lastStateA) {
    lastDebounceTimeA = currentMillis;
  }
  if ((currentMillis - lastDebounceTimeA) > debounceDelay) {
    static int buttonStateA = HIGH;
    if (readingA != buttonStateA) {
      buttonStateA = readingA;
      
      // Nhấn Nút A
      if (buttonStateA == LOW) {
        if (currentState == STOPPED) {
          // Đang DỪNG -> Chuyển sang CHẠY
          currentState = RUNNING;
          startTime = currentMillis; // Ghi nhận mốc thời gian bắt đầu chạy
        } else {
          // Đang CHẠY -> Chuyển sang TẠM DỪNG
          currentState = STOPPED;
          // Tích lũy khoảng thời gian vừa chạy vào elapsedTime
          elapsedTime += (currentMillis - startTime);
        }
      }
    }
  }
  lastStateA = readingA;

  // --------------------------------------------------
  // 2. XỬ LÝ NÚT B: RESET (Chỉ hoạt động khi STOPPED)
  // --------------------------------------------------
  int readingB = digitalRead(BUTTON_B_PIN);
  if (readingB != lastStateB) {
    lastDebounceTimeB = currentMillis;
  }
  if ((currentMillis - lastDebounceTimeB) > debounceDelay) {
    static int buttonStateB = HIGH;
    if (readingB != buttonStateB) {
      buttonStateB = readingB;

      // Nhấn Nút B
      if (buttonStateB == LOW) {
        // Chỉ cho phép Reset khi đồng hồ đang DỪNG (STOPPED)
        if (currentState == STOPPED) {
          elapsedTime = 0;
          updateDisplay(0);
        }
      }
    }
  }
  lastStateB = readingB;

  // --------------------------------------------------
  // 3. TÍNH TOÁN VÀ CẬP NHẬT MÀN HÌNH
  // --------------------------------------------------
  if (currentState == RUNNING) {
    // Tổng thời gian = thời gian đã tích lũy cũ + khoảng thời gian chạy của lượt này
    unsigned long totalDuration = elapsedTime + (currentMillis - startTime);
    updateDisplay(totalDuration);
  }
}

// Hàm tách và định dạng hiển thị MM:SS:ms
void updateDisplay(unsigned long totalMs) {
  unsigned long minutes = (totalMs / 60000);
  unsigned long seconds = (totalMs % 60000) / 1000;
  unsigned long ms      = totalMs % 1000;

  char timeBuffer[17];
  // Format: "  MM:SS:ms   " (Ví dụ: "  01:23:456  ")
  snprintf(timeBuffer, sizeof(timeBuffer), "  %02lu:%02lu:%03lu  ", minutes, seconds, ms);

  lcd.setCursor(0, 1);
  lcd.print(timeBuffer);
}