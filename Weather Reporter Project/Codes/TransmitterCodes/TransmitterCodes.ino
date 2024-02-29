//TRANSMITTER
#include <DHT.h>
#include <RF24.h>
#include <MQ135.h>

#define DHTPIN 6         // DHT22 sensörünün veri pini
#define DHTTYPE DHT22    // Kullanılan DHT sensörünün tipi
DHT dht(DHTPIN, DHTTYPE);

RF24 radio(7, 8); 

const int analogPin = A0;
MQ135 gasSensor = MQ135(analogPin);     

struct SensorData {
  float temperature;
  float humidity;
  float ppm;
};

void setup() {
  Serial.begin(9600);
  radio.begin();
  dht.begin();
}

void loop() {
  SensorData data;
 
  data.temperature = dht.readTemperature();
  data.humidity = dht.readHumidity();
  data.ppm = gasSensor.getPPM();
 

  radio.openWritingPipe(0xF0F0F0F0E1LL); 
  radio.write(&data, sizeof(data));
  
  Serial.println("Veri gönderildi - Sıcaklık: " + String(data.temperature) + "°C, Nem: " + String(data.humidity) + "%, PPM: " + String(data.ppm));
  
  delay(1000);
}