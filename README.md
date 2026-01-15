## 🔌 Pin Mapping & Wiring Configuration

### BME280 (I2C)

| BME280 | ESP32 GPIO | Keterangan |
|------|------------|-----------|
| SDA  | GPIO21 | Default SDA |
| SCL  | GPIO22 | Default SCL |

---

### PMS7003 (UART)

| PMS7003 | ESP32 GPIO | Keterangan |
|--------|------------|-----------|
| RX (kuning) | GPIO17 (TX) | UART |
| TX (biru) | GPIO16 (RX) | UART |
| Power | 5V | Step-down |
| GND | GND | Common ground |

---

### MAX485 (RS485)

| MAX485 | ESP32 GPIO | Keterangan |
|------|------------|-----------|
| RO | GPIO32 | Serial1 RX |
| DI | GPIO33 | Serial1 TX |
| DE / RE | GPIO13 | Kontrol arah (OUTPUT) |
| VCC | 3.3V | Power |
| GND | GND | Common ground |

---

### Multiplexer HW-178

| HW-178 | ESP32 GPIO |
|-------|------------|
| SIG (Analog Out) | GPIO35 |
| S0 | GPIO25 |
| S1 | GPIO26 |
| S2 | GPIO27 |
| S3 | GPIO4 |

---

### Anemometer

| Kabel | Keterangan |
|------|------------|
| Cokelat | Positif → 12V |
| Hitam | Negatif → N |
| Biru | B |
| Kuning | A |

| Power | Keterangan |
|------|------------|
| 12V | Power supply |
| GND | Common ground |

---

### Power Notes

| Komponen | Keterangan |
|--------|------------|
| PMS7003 Power | 5V (step-down) |
| Semua GND | Common ground |

