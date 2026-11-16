// Portable air quality station: ESP32 + MQ135 (air quality) + MQ7 (carbon monoxide).
// Measurements are sent over WiFi to the Blynk app.
// Master's thesis project, International Hellenic University (2021).

#define BLYNK_PRINT Serial   // must come before the Blynk include

#include <math.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Time.h>
#include <TimeLib.h>
#include <WidgetRTC.h>
#include "secrets.h"         // copy include/secrets.example.h to include/secrets.h

BlynkTimer timer;
WidgetRTC rtc;
WidgetMap myMap(V6);         // virtual pin V6: device location on the Blynk map

// Credentials come from include/secrets.h (not committed to the repository)
char auth[] = SECRET_BLYNK_AUTH;
char ssid[] = SECRET_WIFI_SSID;
char pass[] = SECRET_WIFI_PASS;

// Device location shown on the Blynk map: set your own coordinates
const float DEVICE_LAT = 0.0;
const float DEVICE_LON = 0.0;

String currentTime;
String currentDate;
String message;

float RS_gas = 0;
float ratio = 0;
float sensorValue = 0;
float sensor_volt = 0;
float R0 = 7200.0;

// Digital clock display of the time and date, sent to the app
void clockDisplay()
{
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();

  Blynk.virtualWrite(V3, currentTime);   // time
  Blynk.virtualWrite(V2, currentDate);   // date

  // Device location on the map
  int index = 1;
  myMap.location(index, DEVICE_LAT, DEVICE_LON, "IoT-Air quality");
}

BLYNK_WRITE(V1) {
  GpsParam gps(param);
  // Print 7 decimal places for Lat, Lon
  Serial.print("Lat: ");
  Serial.println(gps.getLat(), 7);
  Serial.print("Lon: ");
  Serial.println(gps.getLon(), 7);
  // Print 2 decimal places for Alt, Speed
  Serial.print("Altitude: ");
  Serial.println(gps.getAltitude(), 2);
  Serial.print("Speed: ");
  Serial.println(gps.getSpeed(), 2);
  Serial.println();
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  // Begin synchronizing time
  rtc.begin();

  // Remove all points from the map, then add the device
  myMap.clear();
  int index = 1;
  myMap.location(index, DEVICE_LAT, DEVICE_LON, "IoT-Air quality");

  // Display digital clock every 10 seconds
  timer.setInterval(10000L, clockDisplay);
}

void loop() {
  Blynk.run();
  timer.run();

  // ---- MQ7 (carbon monoxide) on GPIO39 (VN) ----
  sensorValue = analogRead(39);
  sensor_volt = sensorValue / 1024 * 5.0;
  RS_gas = (5.0 - sensor_volt) / sensor_volt;
  ratio = RS_gas / 600;          // 600: reference resistance (R0) found with the calibration sketch
  float x = 1538.46 * ratio;
  float ppm = pow(x, -1.709);
  Serial.print("PPM: ");
  Serial.println(ppm);
  Blynk.virtualWrite(V8, ppm);
  delay(50);

  // ---- MQ135 (air quality) on A0 / GPIO36 (VP) ----
  int Reading = analogRead(A0);
  float sensorVoltage = Reading / 1024.0 * 5.00;
  // Calibrated to the measured sensor voltage (resistive divider).
  // Good air quality is a value of 0-50; the sensor outputs 0.83 V in clean air (0.83 x 60.24 = 50).
  float value = sensorVoltage * 60.24;
  Serial.println(value);
  Blynk.virtualWrite(V9, value);
  delay(1000);

  // Arbitrary thresholds: air quality is qualitative and cannot be measured absolutely.
  // Messages (Greek): Hazardous / Very unhealthy / Unhealthy / Moderate / Good
  if (value > 300) message = "Επικίνδυνη";
  else if (value > 200) message = "Πολύ ανθυγιεινή";
  else if (value > 100) message = "Ανθυγιεινή";
  else if (value > 50) message = "Μέτρια";
  else message = "Καλή";

  Serial.println(message);
  Blynk.virtualWrite(V7, message);
  delay(1000);
}
