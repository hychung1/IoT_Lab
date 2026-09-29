#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

// Cấu hình màn hình OLED SSD1306
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1  // Không dùng chân Reset riêng
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Thông tin WiFi
const char* ssid = "TEN_WIFI_CUA_BAN";
const char* password = "MAT_KHAU_CUA_BAN";

// Cấu hình NTP Server & Múi giờ UTC+7 (Việt Nam)
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600;
const int daylightOffset_sec = 0;

unsigned long lastUpdateDisplay = 0;

// Hàm vẽ biểu tượng tín hiệu WiFi lên OLED
void drawWiFiStatus(bool isConnected) {
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(100, 0);
  
  if (isConnected) {
    display.print("[WF:OK]");
  } else {
    display.print("[WF:--]");
  }
}

// Hàm cập nhật toàn bộ thông tin lên màn hình OLED
void updateOLED() {
  struct tm timeinfo;
  bool timeValid = getLocalTime(&timeinfo);
  bool wifiConnected = (WiFi.status() == WL_CONNECTED);

  display.clearDisplay();

  // 1. Tiêu đề & Trạng thái WiFi
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("NTP CLOCK");
  
  drawWiFiStatus(wifiConnected);
  display.drawFastHLine(0, 11, 128, SSD1306_WHITE); // Đường kẻ ngang phân cách

  if (timeValid) {
    // 2. Dòng 1: Ngay/Thang/Nam
    char dateBuff[20];
    strftime(dateBuff, sizeof(dateBuff), "%d/%m/%Y", &timeinfo);
    
    display.setTextSize(1);
    display.setCursor(30, 20);
    display.print(dateBuff);

    // 3. Dòng 2: Gio/Phut/Giay (Size chữ lớn)
    char timeBuff[20];
    strftime(timeBuff, sizeof(timeBuff), "%H:%M:%S", &timeinfo);
    
    display.setTextSize(2);
    display.setCursor(16, 40);
    display.print(timeBuff);
  } else {
    // Trường hợp chưa đồng bộ được NTP
    display.setTextSize(1);
    display.setCursor(10, 30);
    display.print("Dang cap nhat...");
  }

  display.display(); // Xuất dữ liệu ra màn hình
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Khởi tạo màn hình OLED I2C (Địa chỉ mặc định thường là 0x3C)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("[OLED] Khoi tao That bai! Kiem tra lai day noi I2C."));
    for (;;); // Dừng chương trình nếu không tìm thấy màn hình
  }

  // Hiển thị thông báo ban đầu
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.print("Dang ket noi WiFi...");
  display.display();

  // Kết nối WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  // Khởi tạo NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.google.com");
}

void loop() {
  // Cập nhật màn hình OLED định kỳ mỗi 1 giây (1000ms)
  if (millis() - lastUpdateDisplay >= 1000) {
    lastUpdateDisplay = millis();
    updateOLED();
  }
}