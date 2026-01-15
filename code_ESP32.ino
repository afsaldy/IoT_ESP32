#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <HardwareSerial.h>

// === WiFi ===
const char* ssid = "SSID";
const char* password = "Password";

// === URL Google Script === "https://...../exec"
String serverName = "https://script.google.com/...../exec";

// === BME280 ===
Adafruit_BME280 bme;

// === PMS7003 ===
HardwareSerial pmsSerial(2);  // UART2 (RX=16, TX=17)
struct DataPMS {
  int pm1_0;
  int pm2_5;
  int pm10_0;
};
DataPMS dataPMS;

// === RS485 / Anemometer (MAX485) ===
HardwareSerial rs485Serial(1); // UART1 (RX=32, TX=33)
#define DE_RE 13  // kontrol arah RS485 (HIGH kirim, LOW terima)

// === Multiplexer HW-178 (MICS, MQ131, TGS2611) ===
#define MUX_SIG 35
#define MUX_S0  25
#define MUX_S1  26
#define MUX_S2  27
#define MUX_S3  4

#define CH_MICS_NO2  0
#define CH_MICS_NH3  1
#define CH_MICS_CO   2
#define CH_MQ131     3
#define CH_TGS2611   4

// === Variabel umum ===
const int jumlahData = 20;
const uint32_t SENSOR_TIMEOUT_MS = 60000;          // 60 detik per-sensor
const uint32_t RESTART_INTERVAL   = 3600000UL;     // 1 jam periodik
unsigned long lastRestartTime = 0;                 // dicatat saat boot

// Hasil rata-rata
float hasilTemp, hasilHum, hasilPres;
float hasilPM1, hasilPM25, hasilPM10;
float hasilAngin;
float hasilCO, hasilNH3, hasilNO2;
float hasilOzone, hasilGas;

void setup() {
  Serial.begin(115200);

  // === BME280 ===
  bme.begin(0x76);

  // === PMS7003 ===
  pmsSerial.begin(9600, SERIAL_8N1, 16, 17);
  pmsSerial.setTimeout(200); // batasi blocking readBytes

  // === RS485 (Anemometer) ===
  rs485Serial.begin(4800, SERIAL_8N1, 32, 33);
  rs485Serial.setTimeout(200);
  pinMode(DE_RE, OUTPUT);
  digitalWrite(DE_RE, LOW); // default ke receive

  // === Multiplexer ===
  pinMode(MUX_S0, OUTPUT);
  pinMode(MUX_S1, OUTPUT);
  pinMode(MUX_S2, OUTPUT);
  pinMode(MUX_S3, OUTPUT);

  // === Timestamp restart periodik ===
  lastRestartTime = millis();
}

void loop() {
  connectToWiFi();

  bacaBME280();
  bacaPMS7003();
  bacaAnemometer();
  bacaMICS6814();
  bacaMQ131();
  bacaTGS2611();

  // Kirim ke Google Sheet
  kirimKeGoogleSheet();

  // Restart periodik tiap 1 jam (di akhir loop agar tak potong pengiriman)
  if (millis() - lastRestartTime >= RESTART_INTERVAL) {
    ESP.restart();
  }

  delay(20000); // jeda 20 detik antar upload
}

void connectToWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;

  WiFi.begin(ssid, password);
  unsigned long startAttemptTime = millis();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (millis() - startAttemptTime > 45000) {
      ESP.restart();  // Reset ESP32 jika gagal konek >45s
    }
  }
}

// === Helper MUX ===
void selectMuxChannel(int channel) {
  digitalWrite(MUX_S0, channel & 0x01);
  digitalWrite(MUX_S1, (channel >> 1) & 0x01);
  digitalWrite(MUX_S2, (channel >> 2) & 0x01);
  digitalWrite(MUX_S3, (channel >> 3) & 0x01);
}

// === Fungsi sensor (dengan timeout 60 detik per fungsi) ===
void bacaBME280() {
  unsigned long t0 = millis();
  float t=0,h=0,p=0;
  for (int i=0; i<jumlahData; i++) {
    t += bme.readTemperature();
    h += bme.readHumidity();
    p += bme.readPressure() / 100.0;
    if (millis() - t0 > SENSOR_TIMEOUT_MS) ESP.restart();
    delay(100);
  }
  hasilTemp = t / jumlahData;
  hasilHum  = h / jumlahData;
  hasilPres = p / jumlahData;
}

void bacaPMS7003() {
  unsigned long t0 = millis();
  long pm1=0, pm25=0, pm10=0;
  int count=0;

  while (count < jumlahData) {
    if (millis() - t0 > SENSOR_TIMEOUT_MS) ESP.restart();
    if (bacaFramePMS()) {
      pm1  += dataPMS.pm1_0;
      pm25 += dataPMS.pm2_5;
      pm10 += dataPMS.pm10_0;
      count++;
    }
    delay(250);
  }
  hasilPM1  = (float)pm1  / jumlahData;
  hasilPM25 = (float)pm25 / jumlahData;
  hasilPM10 = (float)pm10 / jumlahData;
}

bool bacaFramePMS() {
  uint8_t buffer[32];
  uint16_t checksum = 0;

  if (pmsSerial.available() < 32) return false;
  if (pmsSerial.read() != 0x42) return false;
  if (pmsSerial.read() != 0x4D) return false;

  buffer[0]=0x42; buffer[1]=0x4D;
  checksum=0x42+0x4D;

  for (int i=2;i<32;i++){
    int c = pmsSerial.read();
    if (c < 0) return false;
    buffer[i]=(uint8_t)c;
    if (i<30) checksum+=buffer[i];
  }

  uint16_t checksumRecv=(buffer[30]<<8)|buffer[31];
  if (checksum!=checksumRecv) return false;

  dataPMS.pm1_0 =(buffer[10]<<8)|buffer[11];
  dataPMS.pm2_5 =(buffer[12]<<8)|buffer[13];
  dataPMS.pm10_0=(buffer[14]<<8)|buffer[15];
  return true;
}

void bacaAnemometer() {
  unsigned long t0 = millis();
  float total=0;
  for (int i=0;i<jumlahData;i++) {
    if (millis() - t0 > SENSOR_TIMEOUT_MS) ESP.restart();

    digitalWrite(DE_RE,HIGH); // transmit
    byte req[] = {0x01,0x03,0x00,0x00,0x00,0x01,0x84,0x0A};
    rs485Serial.write(req,sizeof(req));
    rs485Serial.flush();
    digitalWrite(DE_RE,LOW);  // receive

    byte buf[7];
    if (rs485Serial.readBytes(buf,7)==7) {
      int wind = (buf[3]<<8)|buf[4];
      total += wind/10.0;
    }
    delay(100);
  }
  hasilAngin = total / jumlahData;
}

void bacaMICS6814() {
  unsigned long t0 = millis();
  float co=0, nh3=0, no2=0;
  for (int i=0;i<jumlahData;i++) {
    if (millis() - t0 > SENSOR_TIMEOUT_MS) ESP.restart();

    selectMuxChannel(CH_MICS_CO); delay(10);
    co  += analogRead(MUX_SIG);

    selectMuxChannel(CH_MICS_NH3); delay(10);
    nh3 += analogRead(MUX_SIG);

    selectMuxChannel(CH_MICS_NO2); delay(10);
    no2 += analogRead(MUX_SIG);

    delay(100);
  }
  hasilCO  = co  / jumlahData;
  hasilNH3 = nh3 / jumlahData;
  hasilNO2 = no2 / jumlahData;
}

void bacaMQ131() {
  unsigned long t0 = millis();
  float oz=0;
  for (int i=0;i<jumlahData;i++) {
    if (millis() - t0 > SENSOR_TIMEOUT_MS) ESP.restart();

    selectMuxChannel(CH_MQ131); delay(10);
    oz += analogRead(MUX_SIG);
    delay(50);
  }
  hasilOzone = oz / jumlahData;
}

void bacaTGS2611() {
  unsigned long t0 = millis();
  float gas=0;
  for (int i=0;i<jumlahData;i++) {
    if (millis() - t0 > SENSOR_TIMEOUT_MS) ESP.restart();

    selectMuxChannel(CH_TGS2611); delay(10);
    gas += analogRead(MUX_SIG);
    delay(100);
  }
  hasilGas = gas / jumlahData;
}

// === Kirim ke Google Sheet ===
void kirimKeGoogleSheet() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = serverName + "?suhu=" + String(hasilTemp,2) +
                 "&kelembaban=" + String(hasilHum,2) +
                 "&tekanan=" + String(hasilPres,2) +
                 "&angin=" + String(hasilAngin,2) +
                 "&pm1=" + String(hasilPM1,2) +
                 "&pm25=" + String(hasilPM25,2) +
                 "&pm10=" + String(hasilPM10,2) +
                 "&co=" + String(hasilCO,2) +
                 "&nh3=" + String(hasilNH3,2) +
                 "&no2=" + String(hasilNO2,2) +
                 "&ch4=" + String(hasilGas,2) +       // TGS2611 → CH4 proxy
                 "&ozon=" + String(hasilOzone,2);     // MQ131 → O3 proxy
    http.begin(url.c_str());
    http.GET();
    http.end();
  }
}
