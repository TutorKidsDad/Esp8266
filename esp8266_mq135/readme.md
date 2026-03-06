
# MQ-135 IoT Air Monitor (ESP8266)

## Overview

This project creates a WiFi-based air quality monitoring system using NodeMCU.

## Components Required

* NodeMCU ESP8266
* MQ-135 Sensor Module
* Breadboard
* Jumper wires

---

## Wiring Connections

| MQ-135 | NodeMCU      |
| ------ | ------------ |
| VCC    | VIN (5V)     |
| GND    | GND          |
| A0     | A0           |
| D0     | Not required |

---

## Important Voltage Note

NodeMCU A0 maximum input = **1V**

Most MQ-135 modules include voltage divider.

If not → you MUST add external voltage divider.

---

## Circuit Notes

* ESP8266 ADC range = 0–1023
* Only 1 analog pin available
* Suitable for IoT dashboard projects

---

## IoT Features

* Connect to WiFi
* Web dashboard
* Cloud integration possible (ThingSpeak, Blynk)

---

## Calibration

Typical values:

* Clean Air: 100–250
* Moderate: 250–400
* Poor: 400+

---

## Notes

* WiFi causes slight ADC noise
* Use averaging for stability
* Ensure stable 5V supply to sensor heater

---
