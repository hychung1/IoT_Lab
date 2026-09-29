#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiManager.h>
#include <time.h>
#include <WiFi.h>


// Cấu hình màn hình OLED SSD1306 (128x64)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Cấu hình NTP Server
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // Múi giờ UTC+7
const int daylightOffset_sec = 0;

// Biến quản lý trạng thái
bool isNtpSynced = false;
unsigned long lastDisplayUpdate = 0;
unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 5000; // Kiểm tra WiFi mỗi 5 giây

// Hàm khởi tạo và kết nối WiFi qua WiFiManager
void setupWiFi() {
  WiFiManager wm;
  
  // Hiển thị thông báo lên OLED trong lúc chờ cấu hình WiFi
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("SMART CLOCK BOOTING");
  display.println("--------------------");
  display.println("Connecting WiFi...");
  display.println("AP: SmartClock_AP");
  display.display();

  // Đặt timeout portal 120 giây. Nếu không cấu hình được vẫn tiếp tục chạy bằng RTC nội
  wm.setConfigPortalTimeout(120);

  if (!wm.autoConnect("SmartClock_AP", "12345678")) {
    Serial.println("[WiFi] Khong the ket noi WiFi! Se chay che do Offline (RTC nội).");
  } else {
    Serial.println("[WiFi] Da ket noi WiFi thanh cong!");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
  }
}

// Hàm khởi tạo đồng bộ NTP
void syncNTP() {
  if (WiFi.status() == WL_CONNECTED) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.google.com");
    
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 2000)) { // Chờ tối đa 2 giây
      isNtpSynced = true;
      Serial.println("[NTP] Dong bo thoi gian thanh cong!");
    } else {
      Serial.println("[NTP] Dong bo NTP that bai.");
    }
  }
}

// Hàm hiển thị giao diện lên OLED
void renderOLED() {
  struct tm timeinfo;
  bool hasTime = getLocalTime(&timeinfo, 10); // Lấy thời gian từ RTC nội
  bool wifiOk = (WiFi.status() == WL_CONNECTED);

  display.clearDisplay();

  // 1. THANH TRẠNG THÁI (HEADER)
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("SMART CLOCK");

  // Hiển thị biểu tượng/Trạng thái WiFi
  display.setCursor(80, 0);
  if (wifiOk) {
    display.print("[WF:OK]");
  } else {
    // Cảnh báo mất WiFi
    display.print("[WF:--"); 
  }
  display.drawFastHLine(0, 11, 128, SSD1306_WHITE);

  // 2. NỘI DUNG THỜI GIAN
  if (hasTime) {
    // Dòng 1: Ngày/Tháng/Năm
    char dateBuff[20];
    strftime(dateBuff, sizeof(dateBuff), "%d/%m/%Y", &timeinfo);
    display.setTextSize(1);
    display.setCursor(30, 18);
    display.print(dateBuff);

    // Dòng 2: Giờ:Phút:Giây (Chữ lớn)
    char timeBuff[20];
    strftime(timeBuff, sizeof(timeBuff), "%H:%M:%S", &timeinfo);
    display.setTextSize(2);
    display.setCursor(16, 34);
    display.print(timeBuff);
  } else {
    // Trường hợp vừa khởi động chưa có giờ và không có WiFi
    display.setTextSize(1);
    display.setCursor(10, 30);
    display.print("No Time Data!");
  }

  // 3. CẢNH BÁO MẤT MẠNG (FOOTER)
  if (!wifiOk) {
    display.setTextSize(1);
    display.setCursor(12, 55);
    display.print("! OFFLINE MODE !");
  } else if (!isNtpSynced) {
    display.setTextSize(1);
    display.setCursor(15, 55);
    display.print("Syncing NTP...");
  }

  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Khởi tạo màn hình OLED I2C
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("[OLED] Loi khoi tao OLED!"));
    for (;;);
  }

  // Bật tính năng tự động kết nối lại ngầm của SDK WiFi
  WiFi.setAutoReconnect(true);

  // Khởi tạo WiFi qua WiFiManager
  setupWiFi();

  // Thử đồng bộ NTP lần đầu
  syncNTP();
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Cập nhật giao diện màn hình mỗi 1 giây (1000 ms)
  if (currentMillis - lastDisplayUpdate >= 1000) {
    lastDisplayUpdate = currentMillis;
    renderOLED();
  }

  // 2. Kiểm tra và phục hồi WiFi / NTP ngầm định kỳ mỗi 5 giây
  if (currentMillis - lastWiFiCheck >= wifiCheckInterval) {
    lastWiFiCheck = currentMillis;

    if (WiFi.status() == WL_CONNECTED) {
      // Nếu có mạng nhưng trước đó chưa đồng bộ NTP thành công -> Tiến hành đồng bộ lại
      if (!isNtpSynced) {
        Serial.println("[Watchdog] Phat hien WiFi da khoi phuc. Dang dong bo lai NTP...");
        syncNTP();
      }
    } else {
      // Nếu mất mạng, đánh dấu chưa đồng bộ NTP chuẩn để khi có mạng lại sẽ ép đồng bộ
      isNtpSynced = false;
      Serial.println("[Watchdog] WiFi dang mat ket noi. He thong dang chay bang RTC noi...");
    }
  }
}