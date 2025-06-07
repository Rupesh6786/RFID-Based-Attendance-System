# 📚 RFID-Based ESP32 Attendance System with Google Spreadsheet Integration

![RFID Attendance Banner](images/demo.gif)

This is an IoT-based smart **Attendance System** using an **ESP32**, **MFRC522 RFID Reader**, and a **16x2 I2C LCD Display**. When an RFID card is tapped, the system identifies the student and sends their name, roll number, date, and time to a **Google Spreadsheet** in real time using **Google Apps Script** via **HTTPS**.

---

## 🚀 Features

- 🛂 RFID-based student identification
- 📡 ESP32 with WiFi for Google Sheets connectivity
- 📅 Automatic date and time using NTP
- 📋 Sends data to Google Sheets via Apps Script Web App
- 📺 16x2 I2C LCD with greetings and timestamp
- ❤️ Emojis and animated welcome messages
- 🔒 Secure HTTPS requests with `WiFiClientSecure`

---

## 🖥️ Hardware Required

| Component            | Quantity |
|----------------------|----------|
| ESP32 Dev Board      | 1        |
| MFRC522 RFID Reader  | 1        |
| RFID Tags/Cards      | 2+       |
| I2C 16x2 LCD Display | 1        |
| Breadboard + Wires   | As needed |
| 5V USB Power Supply  | 1        |

---

## 🔌 Circuit Connections

| RFID Module | ESP32 GPIO |
|-------------|------------|
| SDA (SS)    | D4         |
| SCK         | D18        |
| MOSI        | D23        |
| MISO        | D19        |
| RST         | D5         |
| GND         | GND        |
| VCC         | 3.3V       |

| I2C LCD | ESP32 GPIO |
|--------|------------|
| SDA    | D21        |
| SCL    | D22        |
| VCC    | 5V         |
| GND    | GND        |

---

## 🌐 Google Spreadsheet Setup

1. Create a new **Google Spreadsheet**.
2. Open `Extensions > Apps Script` and paste the following:

```javascript
function doGet(e) {
  var sheet = SpreadsheetApp.getActiveSpreadsheet().getActiveSheet();
  var name = e.parameter.name;
  var rollno = e.parameter.rollno;
  var date = new Date();
  
  sheet.appendRow([date, name, rollno]);
  return ContentService.createTextOutput("Success");
}
