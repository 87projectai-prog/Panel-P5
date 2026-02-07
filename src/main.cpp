#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PxMatrix.h>
#include <PrayerTimes.h>
#include <Ticker.h>

// ================= WIFI =================
const char* ssid     = "Leicha_P5";
const char* password = "PANELP5";

// ================= LOKASI =================
// GANTI kalau bukan Jakarta
double latitude  = -6.200000;
double longitude = 106.816666;
double timezone  = 7;   // WIB

// ================= PANEL P5 (Wemos D1 mini) =================
#define P_LAT D0
#define P_A   D5
#define P_B   D6
#define P_C   D7
#define P_OE  D8
#define P_CLK D1

PxMATRIX display(32, 16, P_LAT, P_OE, P_A, P_B, P_C);

// ================= NTP =================
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 7 * 3600, 60000);

// ================= SHOLAT =================
double sholatTimes[7];

// ================= REFRESH DISPLAY =================
Ticker displayTicker;
void displayUpdate() {
  display.display(70);
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  // Init display
  display.begin(8);
  display.setFastUpdate(true);
  display.setBrightness(80);
  display.setColorDepth(1);

  displayTicker.attach_ms(2, displayUpdate);

  // WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // NTP
  timeClient.begin();

  // Metode hisab (umum di Indonesia)
  set_calc_method(Karachi);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
}

// ================= HELPER =================
void printTime(double t) {
  int h = (int)t;
  int m = (t - h) * 60;
  if (h < 10) display.print("0");
  display.print(h);
  display.print(":");
  if (m < 10) display.print("0");
  display.print(m);
}

// ================= LOOP =================
void loop() {
  timeClient.update();

  int hh = timeClient.getHours();
  int mm = timeClient.getMinutes();

  static int lastMinute = -1;
  if (mm != lastMinute) {
    lastMinute = mm;
    get_prayer_times(
      year(), month(), day(),
      latitude, longitude, timezone,
      sholatTimes
    );
  }

  display.clearDisplay();

  // ===== BARIS ATAS =====
  // Jam
  display.setCursor(0, 0);
  if (hh < 10) display.print("0");
  display.print(hh);
  display.print(":");
  if (mm < 10) display.print("0");
  display.print(mm);

  // Dzuhur
  display.setCursor(16, 0);
  display.print("D ");
  printTime(sholatTimes[Dhuhr]);

  // ===== BARIS BAWAH =====
  display.setCursor(0, 8);
  display.print("S ");
  printTime(sholatTimes[Fajr]);

  display.setCursor(11, 8);
  display.print("M ");
  printTime(sholatTimes[Maghrib]);

  display.setCursor(22, 8);
  display.print("I ");
  printTime(sholatTimes[Isha]);

  delay(200);
}
