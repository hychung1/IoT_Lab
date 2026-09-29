#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Ping.h>

// Thông tin WiFi
const char* ssid = "HUIT_SV";
const char* password = "";

// Địa chỉ IP kiểm tra Internet (Google Public DNS)
const IPAddress targetIP(8, 8, 8, 8); 

// Biến quản lý thời gian và bộ đếm lỗi
unsigned long lastPingTime = 0;
// Thời gian giữa mỗi lần kiểm tra: 5 phút = 300,000 ms (Đặt 10,000 ms = 10 giây khi test)
const unsigned long pingInterval = 5 * 60 * 1000; 

int failedPingCount = 0;
const int maxFailedAllowed = 3;

// Hàm kết nối WiFi ban đầu
void connectWiFi() {
  Serial.println("\n[WiFi] Dang ket noi...");
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] Da ket noi WiFi thanh cong!");
    Serial.print("[WiFi] IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n[WiFi] Khong he ket noi duoc WiFi!");
  }
}

// Hàm kiểm tra đường truyền Internet thông qua ICMP Ping
void checkInternetConnection() {
  Serial.println("\n----------------------------------");
  Serial.print("[Watchdog] Dang ping den IP: ");
  Serial.println(targetIP);

  // Chỉ ping khi đã có kết nối WiFi nội bộ
  if (WiFi.status() == WL_CONNECTED) {
    // Thực hiện ping 3 gói tin
    bool pingSuccess = Ping.ping(targetIP, 3);

    if (pingSuccess) {
      Serial.println("[Watchdog] Internet THONG SUOT! Ping thanh cong.");
      // Reset bộ đếm lỗi về 0 khi có 1 lần thành công
      failedPingCount = 0; 
    } else {
      failedPingCount++;
      Serial.print("[Watchdog] PING THAT BAI! So lan loi lien tiep: ");
      Serial.print(failedPingCount);
      Serial.print("/");
      Serial.println(maxFailedAllowed);
    }
  } else {
    failedPingCount++;
    Serial.println("[Watchdog] WiFi dang mat ket noi!");
    Serial.print("[Watchdog] So lan loi lien tiep: ");
    Serial.print(failedPingCount);
    Serial.print("/");
    Serial.println(maxFailedAllowed);
  }

  // Nếu vượt quá số lần cho phép -> Kích hoạt Watchdog Reset
  if (failedPingCount >= maxFailedAllowed) {
    Serial.println("\n[CRITICAL] Phat hien mat Internet lien tuc 3 lan!");
    Serial.println("[CRITICAL] Dang khoi dong lai ESP (ESP.restart())...");
    Serial.flush(); // Chờ in hết chuỗi Serial trước khi reset
    delay(1000);
    ESP.restart(); // Khởi động lại vi điều khiển
  }
  Serial.println("----------------------------------");
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n=== HE THONG WATCHDOG WIFI KHOI DONG ===");
  connectWiFi();

  // Thực hiện kiểm tra ngay 1 lần khi vừa khởi động xong
  checkInternetConnection();
  lastPingTime = millis();
}

void loop() {
  // Kiểm tra định kỳ theo thời gian pingInterval
  if (millis() - lastPingTime >= pingInterval) {
    lastPingTime = millis();
    checkInternetConnection();
  }

  // CÁC TÁC VỤ CHÍNH VẪN HOẠT ĐỘNG BÌNH THƯỜNG Ở ĐÂY
}