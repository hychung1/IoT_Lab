#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <Preferences.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
WebServer server(80);
Preferences preferences;

String ssid = "";
String password = "";

// Giao diện HTML đơn giản nhập WiFi
const char HTML_FORM[] PROGMEM = R"rawliteral(
<!DOCTYPE html><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Cau Hinh WiFi</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; background-color: #f4f4f9; }
    form { display: inline-block; padding: 20px; background: white; border-radius: 8px; box-shadow: 0 0 10px rgba(0,0,0,0.1); }
    input { width: 100%; padding: 10px; margin: 8px 0; box-sizing: border-box; }
    input[type=submit] { background-color: #4CAF50; color: white; border: none; cursor: pointer; }
  </style>
</head>
<body>
  <h2>Cau Hinh WiFi Cho ESP32</h2>
  <form action="/save" method="POST">
    <label>SSID WiFi:</label><br>
    <input type="text" name="ssid" required><br>
    <label>Mat khau:</label><br>
    <input type="password" name="password"><br><br>
    <input type="submit" value="Luu & Ket Noi">
  </form>
</body>
</html>
)rawliteral";

// Lưu thông tin vào Flash và khởi động lại
void handleSave() {
  if (server.hasArg("ssid")) {
    String req_ssid = server.arg("ssid");
    String req_pass = server.arg("password");

    preferences.begin("wifi_config", false);
    preferences.putString("ssid", req_ssid);
    preferences.putString("password", req_pass);
    preferences.end();

    server.send(200, "text/html; charset=utf-8", "<h3>Da luu thong tin! ESP32 dang khoi dong lai...</h3>");
    delay(2000);
    ESP.restart();
  }
}

// Chế độ phát AP và kích hoạt Captive Portal
void startCaptivePortal() {
  Serial.println("\n[Portal] Dang kich hoat Che do Access Point...");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("SmartDevice_AP");

  // Đăng ký DNS bắt mọi tên miền về địa chỉ IP 192.168.4.1
  dnsServer.start(DNS_PORT, "*", apIP);

  // Định tuyến WebServer
  server.on("/", []() { server.send(200, "text/html", HTML_FORM); });
  server.on("/save", HTTP_POST, handleSave);
  server.onNotFound([]() { server.send(200, "text/html", HTML_FORM); });

  server.begin();
  Serial.println("[Portal] AP dang phat: 'SmartDevice_AP'. Vui long ket noi de cau hinh.");
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // Đọc WiFi đã lưu trong bộ nhớ Preferences
  preferences.begin("wifi_config", true);
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  preferences.end();

  if (ssid != "") {
    Serial.print("[WiFi] Tim thay thong tin da luu. Dang ket noi toi SSID: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), password.c_str());

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
      delay(500);
      Serial.print(".");
      retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\n[WiFi] Ket noi thanh cong!");
      Serial.print("[WiFi] IP Address: ");
      Serial.println(WiFi.localIP());
      return; // Kết nối thành công, thoát setup()
    }
    Serial.println("\n[WiFi] Khong the ket noi voi thông tin da luu.");
  }

  // Nếu chưa có thông tin hoặc kết nối thất bại -> Chạy Captive Portal
  startCaptivePortal();
}

void loop() {
  // Nếu đang ở chế độ AP (chưa kết nối thành công WiFi)
  if (WiFi.getMode() == WIFI_AP) {
    dnsServer.processNextRequest();
    server.handleClient();
  }
}