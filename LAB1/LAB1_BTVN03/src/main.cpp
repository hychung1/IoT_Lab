#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// Khai báo chân cắm
#define BUTTON_PIN 18
#define DHTPIN 15
#define DHTTYPE DHT22

// Khởi tạo thiết bị
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Biến quản lý trang hiển thị
int page = 1; // Mặc định bắt đầu ở Trang 1

// Biến cho thuật toán chống dội nút bấm (Debounce)
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Biến đọc cảm biến định kỳ
unsigned long lastSensorReadTime = 0;
const unsigned long sensorReadInterval = 2000; // Đọc DHT22 mỗi 2s
float temperatureC = 0.0;
float humidity = 0.0;

// === KHAI BÁO PROTOTYPE HÀM (Sửa lỗi 'updateUI' was not declared) ===
void updateUI();

void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  dht.begin();
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  // Đọc dữ liệu cảm biến lần đầu
  temperatureC = dht.readTemperature();
  humidity = dht.readHumidity();

  // Hiển thị giao diện trang đầu tiên
  updateUI();
}

void loop() {
  unsigned long currentMillis = millis();

  // ----------------------------------------------------
  // 1. ĐỌC DỮ LIỆU CẢM BIẾN ĐỊNH KỲ (KHÔNG DÙNG DELAY)
  // ----------------------------------------------------
  if (currentMillis - lastSensorReadTime >= sensorReadInterval) {
    lastSensorReadTime = currentMillis;

    float t = dht.readTemperature();
    float h = dht.readHumidity();

    if (!isnan(t) && !isnan(h)) {
      temperatureC = t;
      humidity = h;
    }
  }

  // ----------------------------------------------------
  // 2. XỬ LÝ NÚT BẤM CHUYỂN TRANG (DEBOUNCE)
  // ----------------------------------------------------
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    static int buttonState = HIGH;
    if (reading != buttonState) {
      buttonState = reading;

      // Khi nhấn nút thành công (Edge Detection: HIGH -> LOW)
      if (buttonState = LOW) {
        page++;        // Tăng số trang
        if (page > 3) {
          page = 1;    // Nếu vượt quá trang 3 thì quay lại trang 1
        }
        lcd.clear();   // Xóa màn hình cũ để chuẩn bị in trang mới
      }
    }
  }
  
  // Dùng biến lastButtonState đã khai báo ở đầu file (KHÔNG dùng lastStateA)
  lastButtonState = reading;

  // ----------------------------------------------------
  // 3. CẬP NHẬT GIAO DIỆN MÀN HÌNH THEO TRANG HIỆN TẠI
  // ----------------------------------------------------
  updateUI();
}

// Hàm render giao diện theo giá trị của biến page
void updateUI() {
  char bufferLine2[17];

  switch (page) {
    case 1:
      // TRANG 1: HIỂN THỊ NHIỆT ĐỘ
      lcd.setCursor(0, 0);
      lcd.print("[ TRANG 1/3 ]   ");

      lcd.setCursor(0, 1);
      snprintf(bufferLine2, sizeof(bufferLine2), "Nhiet do:%.1f%cC ", temperatureC, (char)223);
      lcd.print(bufferLine2);
      break;

    case 2:
      // TRANG 2: HIỂN THỊ ĐỘ ẨM
      lcd.setCursor(0, 0);
      lcd.print("[ TRANG 2/3 ]   ");

      lcd.setCursor(0, 1);
      snprintf(bufferLine2, sizeof(bufferLine2), "Do am:   %.1f%%  ", humidity);
      lcd.print(bufferLine2);
      break;

    case 3:
      // TRANG 3: HIỂN THỊ TRẠNG THÁI HỆ THỐNG
      lcd.setCursor(0, 0);
      lcd.print("[ TRANG 3/3 ]   ");

      lcd.setCursor(0, 1);
      lcd.print("Trang thai: OK  ");
      break;
  }
}