//RECEIVER
#include <Arduino.h>
#include <U8g2lib.h>
#include "DHT.h"
#include <MQ135.h>

#define DHTPIN 6     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#define CE_PIN 7
#define CSN_PIN 8
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "000010";

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

const int analogPin = A0;
MQ135 gasSensor = MQ135(analogPin);
const int cleanAirThreshold = 216; 
const int dirtyAirThreshold = 512;

struct SensorData {
  float temperature;
  float humidity;
  float ppm;
};

void setup(void) {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(1, 0xF0F0F0F0E1LL);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  
  dht.begin();
  u8g2.begin();  
}

void loop(void) {
  delay(1000);

  SensorData data;
  radio.read(&data, sizeof(data));

  // Wait to get input from DHT22
  String t = String(data.temperature);
  String h = String(data.humidity);
  float ppm = float(data.ppm);

  Serial.println("Alınan Veri - Sıcaklık: " + t + "°C, Nem: " + h + "% Ppm is " + ppm);

  u8g2.firstPage(); 

  do{
    clearScreen();
  } while (u8g2.nextPage());

  delay(100);

  do {
    inScreenQuality(); 
  } while (u8g2.nextPage());
  
  delay(2000); 

  do{
    clearScreen();
  } while (u8g2.nextPage());

  delay(100);

  u8g2.firstPage();
  do {
    outScreenQuality(ppm); 
  } while (u8g2.nextPage());
  delay(2000);

  do{
    clearScreen();
  } while (u8g2.nextPage());

  delay(100);

  u8g2.firstPage();
  do {
    inScreenTemHum(); 
  } while (u8g2.nextPage());
  delay(2000);

  do{
    clearScreen();
  } while (u8g2.nextPage());

  delay(100);

  u8g2.firstPage();
  do {
    outScreenTemHum(data.temperature, data.humidity); 
  } while (u8g2.nextPage());

  delay(1000);
}

void inScreenQuality(void) {
  float ppm = gasSensor.getPPM();
  String quality = "";

  if (ppm < cleanAirThreshold) {
    quality = "Fresh";
  } else if (ppm < dirtyAirThreshold) {
    quality = "Breathable";
  } else {
    quality = "Dirty";
  }

  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.setCursor(8, 16);
  u8g2.print("Indoor Air Quality: ");
  u8g2.setCursor(8, 38);
  u8g2.print(quality.c_str());
  u8g2.setCursor(8, 60);
  u8g2.print("PPM: "); u8g2.print(ppm);
}

void inScreenTemHum(void) {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.setCursor(8, 16);
  u8g2.print("Indoor Air");
  u8g2.setCursor(8, 38);
  u8g2.print("Tempeture and Humidity");
  u8g2.setCursor(8, 60);
  u8g2.print("T: ");
  u8g2.print(t);
  u8g2.print(char(176));
  u8g2.print("C");
  u8g2.print("H: ");
  u8g2.print(h);
  u8g2.print(" %");
}

void outScreenQuality(float ppm) {
  String quality = "";

  if (ppm < cleanAirThreshold) {
    quality = "Fresh";
  } else if (ppm < dirtyAirThreshold) {
    quality = "Breathable";
  } else {
    quality = "Dirty";
  }

  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.setCursor(8, 16);
  u8g2.print("Outdoor Air Quality");
  u8g2.setCursor(8, 38);
  u8g2.print(quality);
  u8g2.setCursor(8, 60);
  u8g2.print("PPM: ");
  u8g2.print(ppm);
}
void outScreenTemHum(float t, float h) {
  u8g2.setFont(u8g2_font_helvB10_tf);
  u8g2.setCursor(8, 16);
  u8g2.print("Outdoor Air");
  u8g2.setCursor(8, 38);
  u8g2.print("Temp and Hum");
  u8g2.setCursor(8, 60);
  u8g2.print("T: ");
  u8g2.print(t);
  u8g2.print(char(176));
  u8g2.print("C");
  u8g2.print(" H: ");
  u8g2.print(h);
  u8g2.print(" %");
}
void clearScreen(void) {
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 0, u8g2.getWidth(), u8g2.getHeight());
}