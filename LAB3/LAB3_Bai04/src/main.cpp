#include <Arduino.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <WiFi.h>

const char* ssid = "HUIT_SV";
const char* password = "";

// Khai báo UDP client và NTP client
WiFiUDP ntpUDP;
// Cấu hình: NTP Server, Múi giờ UTC+7 = 25200 giây, Khoảng thời gian cập nhật (60.000 ms = 1 phút)
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);

unsigned long lastPrintTime = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  // 1. Kết nối WiFi
  Serial.print("\n[WiFi] Dang ket noi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Ket noi thanh cong!");

  // 2. Khởi động NTP Client
  timeClient.begin();
  timeClient.update();
  Serial.println("[NTP] Da khoi tao NTP Client.");
}

void loop() {
  // Cập nhật chu kỳ NTP
  timeClient.update();

  // In ra thời gian mỗi giây
  if (millis() - lastPrintTime >= 1000) {
    lastPrintTime = millis();

    Serial.print("Thoi gian hien tai: ");
    Serial.println(timeClient.getFormattedTime()); // Trả về định dạng HH:MM:SS
  }
}