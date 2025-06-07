//++ Library required for MFRC522
#include <SPI.h>
#include <MFRC522.h>

//++ Library required for LCD display
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//++ Library required for WiFi
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

//++ Library required to get the esp time
#include "time.h"

//++ Variable declaration for RFID Reader
#define SS_PIN 4
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class

MFRC522::MIFARE_Key key;

// Init array that will store new NUID
byte nuidPICC[4];

//++ Variable declaration for LCD Display
#define LCD_ADDRESS 0x27
#define LCD_COLS 16
#define LCD_ROWS 2
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLS, LCD_ROWS);

//++ Student Database
// char RFID_TAGS[4][4] = {{0xD1, 0xDF, 0x6A, }}
String tagUID[] = {
  "f1361f1c",
  "d1df6a1c",
  "e1039d1c",
  "40107f1d",
  "53e797a7",
  "91575724",
  "4057f21d",
  "43713732",
  "30ad4f1d"
};

String Student_Detail[10][10] = { { "RUPESH_H_THAKUR", "4197" }, { "KHAN_GULAM", "4143" }, { "SAGRIKA_MAAM", "SPECIAL_01" }, { "KHALID_SHAIKH", "4142" }, { "AKASH AMBANI", "52" }, { "HARSH_MISHRA", "42" }, { "ADARSH_GUPTA", "4210" }, { "PREM_VISHWKARMA", "4177" }, { "AYUSH", "4217" } };

// Custom character data for Heart emoji
byte heartEmoji[8] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
};

byte smileEmoji[8] = {
  B00000,
  B10001,
  B00000,
  B00000,
  B10001,
  B01110,
  B00000,
};

// Define the text to scroll
String scrollText = "Welcome to the RFID Based Attendance System, Designed and Crafted by Rupesh";

//++ Variable for WiFi & Google Spreadsheet
const char *SSID = "IRON_MAN_2.4G";
const char *PASSWORD = "Hewlett@123";

String GOOGLE_SCRIPT_ID = "AKfycbwQMBeeXh9quPf2JbQXstyhYEnfQTnBUlK3_rflXdBEMBnuZqUUEiGij7SKrSLUWyiDDQ";
WiFiClientSecure client;

//++ variable for time
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;
struct tm timeinfo;

void setup() {
  lcd.begin();  // initialize the lcd
  lcd.backlight();

  // Load the custom character into CGRAM
  lcd.createChar(0, heartEmoji);
  lcd.createChar(1, smileEmoji);

  Serial.begin(9600);

  SPI.begin();      // Init SPI bus
  rfid.PCD_Init();  // Init MFRC522

  //++ importing the default manufacturer key
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("\n\nThis code scan the MIFARE Classsic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  Serial.println();
  // Clear the LCD
  lcd.clear();

  lcd.setCursor(8, 1);
  lcd.write(byte(1));

  Serial.println("\n\nxx --- INTRO TEXT DISPLAYING --- xx");
  // Print the scrolling text
  for (int i = 0; i < scrollText.length(); i++) {
    if (i == scrollText.length() - 14) {
      break;
    }
    lcd.setCursor(0, 0);
    lcd.print(scrollText.substring(i, i + 16));

    delay(300);  // Adjust the scrolling speed here
  }

  // Clear the LCD
  lcd.clear();

  Serial.println("\n\nxx --- CONNECTING WiFi --- xx");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  Serial.println("Started");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //++ Print Connected Network
  lcd.clear();

  lcd.setCursor(2, 0);
  lcd.print("WiFi Network");

  lcd.setCursor(1, 1);
  lcd.print(SSID);

  delay(3000);

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  lcd.clear();
}

void loop() {
  Morning_time_printing_on_lcd();

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if (!rfid.PICC_ReadCardSerial())
    return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI && piccType != MFRC522::PICC_TYPE_MIFARE_1K && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  String uidString = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
    uidString.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
    uidString.concat(String(rfid.uid.uidByte[i], HEX));
  }
  Serial.println();

  // Check if the UID matches any pre-saved tag
  for (int i = 0; i < sizeof(tagUID) / sizeof(tagUID[0]); i++) {
    if (uidString == tagUID[i]) {
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("WELCOME");
      lcd.setCursor(12, 0);
      // Print the Heart emoji
      lcd.write(byte(0));
      if (i == 0) {
        lcd.setCursor(0, 1);
      } else {
        lcd.setCursor(2, 1);
      }
      lcd.print(Student_Detail[i][0]);
      Serial.println(Student_Detail[i][0]);

      sendData(Student_Detail[i][0], Student_Detail[i][1]);
      break;
    } else {
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("WRONG CARD");
    }
  }


  Serial.println(F("The NUID tag is:"));
  Serial.print(F("In hex: "));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  Serial.print(F("In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  //++ Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();

  delay(500);
}


/**
 * Helper routine to dump a byte array as hex values to Serial. 
 */
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

/**
 * Helper routine to dump a byte array as dec values to Serial.
 */
void printDec(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}

#define USE_SERIAL Serial

//++ Function to set data to google spreadsheet
void sendData(String Student_name, String Roll_no) {
  HTTPClient http;
  // "https://script.google.com/macros/s/AKfycbwt_1Y9gJ9YEuodbNzriZg7T_a3IL7_cJwc-qhutS5xfG_k0fk9pN3PHJ8VeNLsGKRZ/exec?sts=write&name=RUPESH_H_THAKUR&rollno=26"
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?sts=write" + "&name=" + Student_name + "&rollno=" + Roll_no;
  http.begin(url.c_str());
  USE_SERIAL.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      USE_SERIAL.println(payload);
    }
  } else {
    USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

// Function to print the short form of the day name
String getDayShortName(int day) {
  switch (day) {
    case 0: return "Sun";
    case 1: return "Mon";
    case 2: return "Tue";
    case 3: return "Wed";
    case 4: return "Thu";
    case 5: return "Fri";
    case 6: return "Sat";
    default: return "";
  }
}

void Morning_time_printing_on_lcd() {
  getLocalTime(&timeinfo);

  // Determine if it's morning, afternoon, or evening
  float hour = timeinfo.tm_hour;
  float min = timeinfo.tm_min;

  float current_time = hour + (min / 60) + 5.5;

  const char *timeOfDay;

  if (current_time >= 5 && current_time < 12) {
    timeOfDay = "Good Morning ";
  } else if (current_time >= 12 && current_time < 18) {
    timeOfDay = "Good Afternoon ";
  } else {
    timeOfDay = "Good Evening ";
  }

  // Print "Good Morning" with a smiley on the first line
  lcd.setCursor(0, 0);
  lcd.print(timeOfDay);

  lcd.write(byte(1));

  // Print date and time on the second line
  lcd.setCursor(0, 1);
  lcd.print(getDayShortName(timeinfo.tm_wday));
  lcd.print(" ");
  lcd.print(String(timeinfo.tm_mday));  // Print date
  lcd.print(" ");

  if (timeinfo.tm_min + 30 > 60) {
    lcd.print(String(timeinfo.tm_hour + 6) + ":" + String(timeinfo.tm_min - 30) + ":" + String(timeinfo.tm_sec));  // Print time
  } else {
    lcd.print(String(timeinfo.tm_hour + 5) + ":" + String(timeinfo.tm_min + 30) + ":" + String(timeinfo.tm_sec));  // Print time
  }
}

//https://docs.google.com/spreadsheets/d/1j9o4xE8tX_VLOn5CtJMJdP4spthES2jAP42M3FwaG3M/edit?gid=0#gid=0