#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <DHT.h>
#include <MQ135.h>

char auth[] = "<authkey>";
char ssid[] = "<wifissid>";
char pass[] = "<wifipassword>";

#include <SoftwareSerial.h>
SoftwareSerial EspSerial(A0, A1); // RX, TX
#define ESP8266_BAUD 9600
ESP8266 wifi(&EspSerial);

#define DHTPIN A4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define PIN_MQ135 A2
MQ135 mq135_sensor = MQ135(PIN_MQ135);

BlynkTimer timer;

float h = 25.0;
float t = 28.0;
float correctedPPM = 0.0;

void sendSensor()
{
  h = dht.readHumidity();
  t = dht.readTemperature();
  if (isnan(h) || isnan(t))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  correctedPPM = mq135_sensor.getCorrectedPPM(t, h);
  correctedPPM = correctedPPM * 8000;

  Blynk.virtualWrite(V2, correctedPPM);
  if (correctedPPM <= 500)
  {
    Serial.println(correctedPPM);
    Blynk.virtualWrite(V3, correctedPPM);
    Blynk.virtualWrite(V4, 0);
    Blynk.virtualWrite(V5, 0);
  }

  else if (correctedPPM <= 5000)
  {
    Serial.println(correctedPPM);
    Blynk.virtualWrite(V3, (correctedPPM / 2) + (correctedPPM / 1000));
    Blynk.virtualWrite(V4, (correctedPPM / 2));
    Blynk.virtualWrite(V5, 0);
  }

  else if (correctedPPM > 5000)
  {
    Serial.println(correctedPPM);
    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, (correctedPPM / 2) + (correctedPPM / 1000));
    Blynk.virtualWrite(V5, (correctedPPM / 2));
  }
}

void setup()
{
  pinMode(A3, OUTPUT);
  pinMode(A5, OUTPUT);
  digitalWrite(A3, HIGH);
  digitalWrite(A5, LOW);

  Serial.begin(9600);

  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  Blynk.begin(auth, wifi, ssid, pass);

  dht.begin();

  timer.setInterval(1000L, sendSensor);
}

void loop()
{
  Blynk.run();
  timer.run();
}
