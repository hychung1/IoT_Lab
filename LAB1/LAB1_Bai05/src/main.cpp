#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Khởi tạo LCD với địa chỉ I2C thường gặp (0x27), kích thước 16 cột và 2 dòng
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // Khởi tạo kết nối I2C trên ESP32 (SDA = 21, SCL = 22)
  Wire.begin(21, 22);

  // Khởi tạo LCD và bật đèn nền
  lcd.init();
  lcd.backlight();

  // In dòng tiêu đề cố định ở Dòng 1 (Cột 0, Dòng 0)
  lcd.setCursor(0, 0);
  lcd.print("Thoi gian chay:");
}

void loop() {
  // 1. Lấy số giây trôi qua bằng cách chia millis() cho 1000
  unsigned long uptimeSeconds = millis() / 1000;

  // 2. Di chuyển con trỏ xuống Dòng 2 (Cột 0, Dòng 1)
  lcd.setCursor(0, 1);

  // 3. Sử dụng sprintf để định dạng chuỗi, tự động xóa ký tự thừa
  // %-4lu: In số nguyên không dấu (unsigned long), canh trái, dành sẵn ít nhất 4 khoảng trắng
  char buffer[17];
  snprintf(buffer, sizeof(buffer), "%-4lu giay", uptimeSeconds);

  // 4. In chuỗi đã định dạng lên LCD
  lcd.print(buffer);

  // Cập nhật lại màn hình mỗi 100ms
  delay(100);
}