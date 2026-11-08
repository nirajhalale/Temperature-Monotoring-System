/*
  Temperature & Humidity Monitoring System
  Board: ESP32 Dev Module
  Sensors: DHT11 + DS18B20
  Display: 16x2 I2C LCD

  Serial output format:
  DATA,<temperature_dht_c>,<humidity_percent>,<temperature_ds18b20_c>
*/
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11
#define DS18B20_PIN 5
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_ROWS 2

DHT dht(DHT_PIN, DHT_TYPE);
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);
LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

unsigned long lastRead = 0;
const unsigned long READ_INTERVAL_MS = 2000;

void showStartup() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(1500);
  lcd.clear();
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  dht.begin();
  ds18b20.begin();
  lcd.init();
  lcd.backlight();
  showStartup();

  Serial.println("Temperature Monitoring System");
  Serial.println("Format: DATA,dht_c,humidity,ds18b20_c");
}

void loop() {
  if (millis() - lastRead < READ_INTERVAL_MS) return;
  lastRead = millis();

  float humidity = dht.readHumidity();
  float dhtTemp = dht.readTemperature();

  ds18b20.requestTemperatures();
  float dsTemp = ds18b20.getTempCByIndex(0);

  bool dhtValid = !isnan(humidity) && !isnan(dhtTemp);
  bool dsValid = dsTemp != DEVICE_DISCONNECTED_C && dsTemp > -55.0 && dsTemp < 125.0;

  lcd.clear();
  if (dhtValid) {
    lcd.setCursor(0, 0);
    lcd.print("DHT:");
    lcd.print(dhtTemp, 1);
    lcd.print((char)223);
    lcd.print("C ");
    lcd.print(humidity, 0);
    lcd.print("%");
  } else {
    lcd.setCursor(0, 0);
    lcd.print("DHT11 read err");
  }

  lcd.setCursor(0, 1);
  if (dsValid) {
    lcd.print("DS18B20:");
    lcd.print(dsTemp, 1);
    lcd.print((char)223);
    lcd.print("C");
  } else {
    lcd.print("DS18B20 error");
  }

  if (dhtValid && dsValid) {
    Serial.print("DATA,");
    Serial.print(dhtTemp, 2);
    Serial.print(",");
    Serial.print(humidity, 2);
    Serial.print(",");
    Serial.println(dsTemp, 2);
  } else {
    Serial.println("ERROR,One or more sensor readings invalid");
  }
}
