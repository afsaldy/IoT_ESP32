# ESP32 Environmental Monitoring System

Proyek ini menggunakan **ESP32** sebagai pusat akuisisi data dari beberapa sensor lingkungan, termasuk sensor suhu & kelembapan, sensor partikel udara, komunikasi RS485, serta pembacaan sensor analog menggunakan multiplexer.

---

## 🧩 Hardware Configuration

### 🔌 Microcontroller
- **ESP32 Dev Module**

---

## 🧭 I2C – BME280 (Temperature, Humidity, Pressure)

BME280 terhubung ke ESP32 menggunakan protokol **I2C** dengan pin default ESP32.

| BME280 | ESP32 GPIO | Keterangan |
|------|-----------|-----------|
| SDA  | GPIO21 | I2C Data (default SDA) |
| SCL  | GPIO22 | I2C Clock (default SCL) |
| VCC  | 3.3V | Power |
| GND  | GND | Common ground |

---

## 🌫️ UART – PMS7003 (Particulate Matter Sensor)

PMS7003 menggunakan komunikasi **UART** dan membutuhkan supply **5V** (menggunakan step-down converter).

| PMS7003 | ESP32 GPIO | Keterangan |
|--------|-----------|-----------|
| RX (kuning) | GPIO17 (TX) | UART TX ESP32 |
| TX (biru) | GPIO16 (RX) | UART RX ESP32 |
| VCC | 5V | Power (step-down) |
| GND | GND | Common ground |

---

## 🔌 RS485 – MAX485 Module

MAX485 digunakan untuk komunikasi **RS485** dengan kontrol arah data (TX / RX).

| MAX485 | ESP32 GPIO | Keterangan |
|------|-----------|-----------|
| RO | GPIO32 | Serial1 RX |
| DI | GPIO33 | Serial1 TX |
| DE / RE | GPIO13 | Direction Control (OUTPUT) |
| VCC | 3.3V | Power |
| GND | GND | Common ground |

---

## 🔀 Analog Multiplexer – HW-178 (CD74HC4067)

Multiplexer digunakan untuk membaca beberapa input analog menggunakan satu pin ADC ESP32.

| HW-178 | ESP32 GPIO | Keterangan |
|-------|-----------|-----------|
| SIG | GPIO35 | Analog Output (ADC) |
| S0 | GPIO25 | Channel Select |
| S1 | GPIO26 | Channel Select |
| S2 | GPIO27 | Channel Select |
| S3 | GPIO4 | Channel Select |
| VCC | 3.3V | Power |
| GND | GND | Common ground |

---

## 🌬️ Anemometer (Wind Speed Sensor)

- **Power Supply** : 12V (catu daya terpisah)
- **Signal Output** : Terhubung ke multiplexer HW-178
- **GND** : Disatukan dengan GND ESP32 (common ground)

---

## ⚠️ Important Notes

> **Semua GND harus disatukan (common ground)**

Hal ini penting untuk:
- Menjaga referensi tegangan yang sama
- Menghindari error komunikasi UART & RS485
- Menghasilkan pembacaan sensor yang stabil

---

## 🧠 Interface Summary

- **I2C**  → BME280  
- **UART** → PMS7003  
- **UART (Serial1)** → MAX485 (RS485)  
- **ADC + Multiplexer** → Anemometer & sensor analog lainnya  

---

## 📁 Project Purpose

Sistem ini dirancang untuk:
- Monitoring kondisi lingkungan
- Akuisisi data sensor secara real-time
- Integrasi berbagai protokol komunikasi pada ESP32

---

## 📌 Notes for Development

- Pastikan level tegangan sesuai (3.3V vs 5V)
- Gunakan step-down converter untuk PMS7003
- Periksa kembali pin mapping sebelum upload firmware

---

