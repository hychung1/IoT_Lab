#include <Arduino.h>
#include "DHT.h"

// Định nghĩa chân cắm
#define BUTTON_PIN 18
#define LED1_PIN 4
#define LED2_PIN 5
#define DHTPIN 23

// Đổi thành DHT11 nếu chạy trên phần cứng thực tế
#define DHTTYPE DHT22  

// Khởi tạo đối tượng cảm biến DHT
DHT dht(DHTPIN, DHTTYPE);

// ----------------------------------------------------
// BIẾN QUẢN LÝ CẢM BIẾN DHT
// ----------------------------------------------------
unsigned long lastDHTReadTime = 0;
const unsigned long dhtInterval = 2000; // Đọc cảm biến mỗi 2s (2000ms)

// ----------------------------------------------------
// BIẾN QUẢN LÝ NÚT BẤM VÀ LED (DEBOUNCE)
// ----------------------------------------------------
bool ledState = false;
int lastButtonReading = HIGH;
int stableButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);

  digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
  digitalWrite(LED2_PIN, ledState ? HIGH : LOW);

  dht.begin();
  Serial.println("=== HE THONG PHAN LUONG: DHT + NUT BAM ===");
}

void loop() {
  unsigned long currentMillis = millis();

  // ====================================================
  // TIẾN TRÌNH 1: ĐỌC CẢM BIẾN DHT ĐỊNH KỲ (MỖI 2 GIÂY)
  // ====================================================
  if (currentMillis - lastDHTReadTime >= dhtInterval) {
    lastDHTReadTime = currentMillis;

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (isnan(temp) || isnan(hum)) {
      Serial.println("[LOI] Khong doc duoc du lieu tu cam bien DHT!");
    } else {
      Serial.print("[DHT] Nhiet do: ");
      Serial.print(temp, 1);
      Serial.print(" °C | Do am: ");
      Serial.print(hum, 1);
      Serial.println(" %");
    }
  }

  // ====================================================
  // TIẾN TRÌNH 2: XỬ LÝ NÚT BẤM BẬT/TẮT LED TỨC THÌ
  // ====================================================
  int currentReading = digitalRead(BUTTON_PIN);

  if (currentReading != lastButtonReading) {
    lastDebounceTime = currentMillis;
  }

  if ((currentMillis - lastDebounceTime) > debounceDelay) {
    if (currentReading != stableButtonState) {
      stableButtonState = currentReading;

      // Sự kiện nhấn nút (Cạnh xuống: HIGH -> LOW)
      if (stableButtonState == LOW) {
        ledState = !ledState;
        digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
        digitalWrite(LED2_PIN, ledState ? HIGH : LOW);
        Serial.print("-> [NUT BAM] Da dao trang thai LED sang: ");
        Serial.println(ledState ? "BAT" : "TAT");
      }
    }
  }

  lastButtonReading = currentReading;
}