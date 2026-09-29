#include <Arduino.h>
#include <time.h>
#include <WiFi.h>

// Thông tin WiFi
const char* ssid = "TEN_WIFI_CUA_BAN";
const char* password = "MAT_KHAU_CUA_BAN";

// Cấu hình NTP
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // Múi giờ UTC+7 (Việt Nam)
const int daylightOffset_sec = 0;

// Chu kỳ đồng bộ định kỳ: 6 tiếng = 6 * 3600 * 1000 ms
// (LƯU Ý: Để test thử nhanh, bạn có thể chỉnh thành 30000 ms = 30 giây)
const unsigned long syncInterval = 6 * 3600 * 1000UL; 
unsigned long lastSyncMillis = 0;

int lastSyncedDay = -1; // Biến ghi nhớ ngày vừa đồng bộ để bắt mốc 00:00:00

// Hàm lấy thời gian Unix epoch hiện tại (tính bằng giây)
time_t getCurrentEpoch() {
  time_t now;
  time(&now);
  return now;
}

// Hàm đồng bộ NTP và đo đạc độ lệch thời gian
bool syncNTPWithDriftLog() {
  Serial.println("\n--------------------------------------------------");
  Serial.println("[NTP Sync] Bat dau qua trinh dong bo lai thoi gian...");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[NTP Sync] LOI: WiFi chua ket noi! Khong the dong bo NTP.");
    return false;
  }

  // 1. Ghi nhận thời gian Unix epoch của RTC nội BÊN TRONG chip TRƯỚC KHI ĐỒNG BỘ
  time_t epochBefore = getCurrentEpoch();

  // 2. Gọi lại configTime để kích hoạt yêu cầu lấy gói tin mới từ NTP Server
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.google.com");

  // 3. Chờ phản hồi từ NTP Server (Timeout 10 giây)
  struct tm timeinfo;
  int retry = 0;
  while (!getLocalTime(&timeinfo) && retry < 20) {
    delay(500);
    Serial.print(".");
    retry++;
  }

  if (retry >= 20) {
    Serial.println("\n[NTP Sync] LOI: Dong bo NTP thoi gian het han (Timeout)!");
    return false;
  }

  // 4. Ghi nhận thời gian Unix epoch nhận được TỪ NTP SERVER SAU KHI ĐỒNG BỘ
  time_t epochAfter = getCurrentEpoch();

  // 5. Tính toán chênh lệch thời gian (Drift)
  long timeDriftSec = (long)(epochAfter - epochBefore);

  // In thông số ra Log Serial
  char timeBuff[30];
  strftime(timeBuff, sizeof(timeBuff), "%Y-%m-%d %H:%M:%S", &timeinfo);

  Serial.println("\n[NTP Sync] Dong bo thanh cong!");
  Serial.print("[NTP Sync] Thoi gian hien tai (Post-Sync): ");
  Serial.println(timeBuff);
  Serial.print("[NTP Sync] Do lech thoi gian (Time Drift): ");
  Serial.print(timeDriftSec);
  Serial.println(" giay.");
  Serial.println("--------------------------------------------------");

  lastSyncedDay = timeinfo.tm_mday; // Cập nhật ngày vừa đồng bộ thành công
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n=== AUTO NTP RE-SYNC & DRIFT MONITOR ===");

  // Kết nối WiFi
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);

  Serial.print("[WiFi] Dang ket noi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Da ket noi thanh cong!");

  // Lần đồng bộ NTP đầu tiên khi khởi động
  syncNTPWithDriftLog();
  lastSyncMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();

  // --- CƠ CHẾ 1: Cập nhật định kỳ mỗi 6 tiếng dựa theo millis() ---
  if (currentMillis - lastSyncMillis >= syncInterval) {
    lastSyncMillis = currentMillis;
    Serial.println("\n[Schedule] Kich hoat dong bo theo chu ky 6 tieng.");
    syncNTPWithDriftLog();
  }

  // --- CƠ CHẾ 2: Cập nhật vào mốc 00:00:00 hàng ngày ---
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    // Kiểm tra nếu là 0 giờ, 0 phút và ngày hôm nay chưa được đồng bộ ở mốc 00:00
    if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0 && timeinfo.tm_mday != lastSyncedDay) {
      Serial.println("\n[Schedule] Kich hoat dong bo vao moc 00:00:00 dem.");
      if (syncNTPWithDriftLog()) {
        lastSyncMillis = millis(); // Reset lại timer 6 tiếng
      }
    }
  }

  // In ra thời gian thực mỗi 2 giây để quan sát
  static unsigned long lastPrint = 0;
  if (currentMillis - lastPrint >= 2000) {
    lastPrint = currentMillis;
    if (getLocalTime(&timeinfo)) {
      char buff[20];
      strftime(buff, sizeof(buff), "%H:%M:%S", &timeinfo);
      Serial.print("[RTC Internal] Current Time: ");
      Serial.println(buff);
    }
  }
}