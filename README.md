# Portable Air Quality Monitoring Station (ESP32 + Blynk)

A battery-powered, portable air quality station built around an **ESP32**. It measures air quality and carbon monoxide and sends the readings over WiFi to the **Blynk** mobile app, so people can see which areas are more polluted. Several devices can form a network covering a neighbourhood, a municipality or a larger area.

This was my **master's thesis project** (Applied Electronic Systems, International Hellenic University, 2021): *"Construction of a network of portable air quality measuring stations"*.

## Hardware

- ESP32 development board
- MQ135 air quality sensor
- MQ7 carbon monoxide (CO) sensor
- Power: battery or power bank

Other sensors I evaluated during the thesis: CJMCU-6814, CCS811, BME280, DHT11.

## Software

- C++ (Arduino framework) built with [PlatformIO](https://platformio.org/)
- [Blynk](https://blynk.io/) for the mobile dashboard
- Libraries: Blynk, Time (see `platformio.ini`)

## What it does

- Reads the MQ7 sensor and converts the value to CO in ppm.
- Reads the MQ135 sensor and converts it to an air quality value, then classifies it as Good, Moderate, Unhealthy, Very unhealthy or Hazardous (labels are shown in Greek in the app).
- Sends the readings, current date and time, and the device location to Blynk.

### Blynk virtual pins

| Pin | Content |
|-----|---------|
| V2 | Date |
| V3 | Time |
| V6 | Device location (map widget) |
| V7 | Air quality message |
| V8 | Carbon monoxide (ppm) |
| V9 | Air quality value (MQ135) |

## Getting started

1. Install [PlatformIO](https://platformio.org/install) (VS Code extension or CLI).
2. Copy `include/secrets.example.h` to `include/secrets.h` and fill in your WiFi name, WiFi password and Blynk auth token. This file is ignored by Git, so your credentials are never uploaded.
3. Set `DEVICE_LAT` and `DEVICE_LON` in `src/main.cpp`.
4. Connect the sensors: MQ7 output to GPIO39 (VN) and MQ135 output to GPIO36 (VP), through a voltage divider.
5. Build and upload with PlatformIO, and open the serial monitor at 9600 baud.

## Notes and limitations

- This is an academic prototype. The air quality thresholds are arbitrary and the sensors need proper calibration before the values can be treated as official measurements.
- The project was written in 2020-2021 for the Blynk platform available at that time. Blynk has since moved to a newer version, so running it today may require migrating the Blynk setup (template and device) and small code changes.
- The PlatformIO configuration in this repository was reconstructed and may need small adjustments for your library versions.

## Author

Anestis Taousianis, Electronic Engineer (MSc)
[LinkedIn](https://linkedin.com/in/anestis-taousianis)
