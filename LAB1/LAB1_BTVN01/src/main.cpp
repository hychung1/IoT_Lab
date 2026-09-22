#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// Định nghĩa chân cắm
#define DHTPIN 15
#define DHTTYPE DHT22
#define BUTTON_PIN 18
#define LED_PIN 23

// Khởi tạo thiết bị
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Biến quản lý trạng thái hiển thị nhiệt độ
bool isFahrenheit = false;
unsigned long modeStartTime = 0;
const unsigned long DISPLAY_F_DURATION = 5000; // 5000ms = 5 giây

// Biến cho thuật toán chống dội nút bấm (Debounce)
int lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Biến lưu thời gian đọc cảm biến định kỳ
unsigned long lastSensorReadTime = 0;
const unsigned long sensorReadInterval = 2000; // Đọc DHT22 mỗi 2 giây

// Biến lưu dữ liệu cảm biến
float temperatureC = 0.0;
float humidity = 0.0;

void setup() {
  Serial.begin(115200);

  // Cấu hình chân GPIO
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Khởi tạo cảm biến và LCD
  dht.begin();
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  // Đọc dữ liệu lần đầu tiên
  temperatureC = dht.readTemperature();
  humidity = dht.readHumidity();
}

void loop() {
  unsigned long currentMillis = millis();

  // ----------------------------------------------------
  // 1. XỬ LÝ ĐỌC DỮ LIỆU CẢM BIẾN ĐỊNH KỲ (KHÔNG DÙNG DELAY)
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
  // 2. XỬ LÝ NÚT BẤM (DEBOUNCE + CHUYỂN CHẾ ĐỘ HIỂN THỊ)
  // ----------------------------------------------------
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonState) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    static int buttonState = HIGH;
    if (reading != buttonState) {
      buttonState = reading;

      // Khi nhấn nút thành công (chuyển từ HIGH sang LOW)
      if (buttonState == LOW) {
        isFahrenheit = true;
        modeStartTime = currentMillis; // Ghi lại mốc thời gian bắt đầu chuyển sang °F
        lcd.clear();                   // Xóa màn hình để cập nhật định dạng mới
      }
    }
  }
  lastButtonState = reading;

  // ----------------------------------------------------
  // 3. TỰ ĐỘNG CHUYỂN VỀ °C SAU 5 GIÂY (DÙNG MILLIS)
  // ----------------------------------------------------
  if (isFahrenheit && (currentMillis - modeStartTime >= DISPLAY_F_DURATION)) {
    isFahrenheit = false;
    lcd.clear(); // Xóa màn hình khi chuyển lại về °C
  }

  // ----------------------------------------------------
  // 4. BẬT/TẮT MÁY HÚT ẨM (ĐÈN LED) KHI ĐỘ ẨM > 80%
  // ----------------------------------------------------
  if (humidity > 80.0) {
    digitalWrite(LED_PIN, HIGH); // Độ ẩm > 80% -> Bật đèn/máy hút ẩm
  } else {
    digitalWrite(LED_PIN, LOW);  // Độ ẩm <= 80% -> Tắt
  }

  // ----------------------------------------------------
  // 5. CẬP NHẬT HIỂN THỊ LÊN MÀN HÌNH LCD
  // ----------------------------------------------------
  char bufferLine1[17];
  char bufferLine2[17];

  lcd.setCursor(0, 0);
  if (isFahrenheit) {
    // Công thức tính Fahrenheit: F = (C * 1.8) + 32
    float temperatureF = (temperatureC * 1.8) + 32.0;
    snprintf(bufferLine1, sizeof(bufferLine1), "Nhiet do:%.1f%cF", temperatureF, (char)223);
  } else {
    snprintf(bufferLine1, sizeof(bufferLine1), "Nhiet do:%.1f%cC", temperatureC, (char)223);
  }
  lcd.print(bufferLine1);

  lcd.setCursor(0, 1);
  snprintf(bufferLine2, sizeof(bufferLine2), "Do am:   %.1f%%  ", humidity);
  lcd.print(bufferLine2);
}