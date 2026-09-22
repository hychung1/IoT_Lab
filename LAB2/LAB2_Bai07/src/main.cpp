#include <Arduino.h>
#include "DHT.h"

// Định nghĩa chân cắm
#define DHTPIN 23
#define BUZZER_PIN 15

// Loại cảm biến (dùng DHT22 cho Wokwi, đổi thành DHT11 nếu chạy phần cứng thật)
#define DHTTYPE DHT22

// Ngưỡng cảnh báo nhiệt độ (°C)
const float TEMP_THRESHOLD = 30.0;

// Khởi tạo đối tượng cảm biến DHT
DHT dht(DHTPIN, DHTTYPE);

// ----------------------------------------------------
// BIẾN CHO TÁC VỤ 1: ĐỌC CẢM BIẾN DHT
// ----------------------------------------------------
unsigned long lastSensorReadTime = 0;
const unsigned long SENSOR_INTERVAL = 2000; // Đọc cảm biến mỗi 2s
float currentTemp = 0.0;
bool isAlarmActive = false; // Trạng thái cho phép còi kêu

// ----------------------------------------------------
// BIẾN CHO TÁC VỤ 2: CẢNH BÁO CÒI NGẮT QUÃNG
// ----------------------------------------------------
unsigned long lastBuzzerToggleTime = 0;
const unsigned long BUZZER_BEEP_INTERVAL = 200; // Bật 200ms, Tắt 200ms
int buzzerState = LOW;

// Khai báo prototype các hàm
void taskReadSensor(unsigned long currentMillis);
void taskBuzzerAlarm(unsigned long currentMillis);

void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  dht.begin();
  Serial.println("=== HE THONG CANH BAO NHIET DO TU DONG ===");
}

void loop() {
  unsigned long currentMillis = millis();

  // Thực thi 2 tác vụ song song
  taskReadSensor(currentMillis);
  taskBuzzerAlarm(currentMillis);
}

// ----------------------------------------------------
// TÁC VỤ 1: Đọc cảm biến DHT22 định kỳ 2 giây
// ----------------------------------------------------
void taskReadSensor(unsigned long currentMillis) {
  if (currentMillis - lastSensorReadTime >= SENSOR_INTERVAL) {
    lastSensorReadTime = currentMillis;

    float t = dht.readTemperature();

    if (isnan(t)) {
      Serial.println("[LOI] Khong doc duoc du lieu tu DHT!");
      return;
    }

    currentTemp = t;
    Serial.print("[DHT22] Nhiet do hien tai: ");
    Serial.print(currentTemp, 1);
    Serial.println(" °C");

    // Kiểm tra ngưỡng cảnh báo nhiệt độ
    if (currentTemp > TEMP_THRESHOLD) {
      if (!isAlarmActive) {
        isAlarmActive = true;
        Serial.println(" -> [CANH BAO] Nhiet do VUOT NGUNG (>30°C)! Kich hoat coi alarm.");
      }
    } else {
      if (isAlarmActive) {
        isAlarmActive = false;
        Serial.println(" -> [AN TOAN] Nhiet do binh thuong (<=30°C). Tat coi alarm.");
      }
    }
  }
}

// ----------------------------------------------------
// TÁC VỤ 2: Đột ngắt quãng còi cảnh báo (Bật 200ms - Tắt 200ms)
// ----------------------------------------------------
void taskBuzzerAlarm(unsigned long currentMillis) {
  if (isAlarmActive) {
    // Nếu trạng thái cảnh báo đang BẬT: tạo nhịp kêu ngắt quãng
    if (currentMillis - lastBuzzerToggleTime >= BUZZER_BEEP_INTERVAL) {
      lastBuzzerToggleTime = currentMillis;

      buzzerState = (buzzerState == LOW) ? HIGH : LOW;
      digitalWrite(BUZZER_PIN, buzzerState);
    }
  } else {
    // Nếu nhiệt độ an toàn: cưỡng chế tắt còi hoàn toàn
    if (buzzerState != LOW) {
      buzzerState = LOW;
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}