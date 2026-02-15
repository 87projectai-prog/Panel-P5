#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_GFX.h>
#include <RGBmatrixPanel.h>

// ===== PIN CONFIG =====
#define CLK 1
#define LAT 20
#define OE  21

#define A 8
#define B 9
#define C 10
#define D 0

#define R1 2
#define G1 3
#define B1 4
#define R2 5
#define G2 6
#define B2 7

WebServer server(80);
RGBmatrixPanel *matrix;

String text = "87PROJECT";
int speedScroll = 40;
uint16_t textColor;
int x;

void handleRoot() {
  String page = "<html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  page += "<style>body{background:black;color:lime;font-family:Arial;text-align:center;}</style></head><body>";
  page += "<h2>P5 CONTROL</h2>";
  page += "<form action='/set'>";
  page += "Text:<br><input name='msg'><br><br>";
  page += "Speed (10-100):<br><input name='spd' type='number'><br><br>";
  page += "<input type='submit' value='UPDATE'>";
  page += "</form></body></html>";
  server.send(200, "text/html", page);
}

void handleSet() {
  if(server.hasArg("msg")) text = server.arg("msg");
  if(server.hasArg("spd")) speedScroll = server.arg("spd").toInt();
  x = 64;
  server.sendHeader("Location","/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  // ===== START WIFI DULU =====
  WiFi.mode(WIFI_AP);
  WiFi.softAP("P5-CONTROL", "12345678");
  Serial.println("WiFi AP Started");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();

  delay(2000); // kasih waktu WiFi stabil

  // ===== BARU START PANEL =====
  matrix = new RGBmatrixPanel(A, B, C, D, CLK, LAT, OE, false, 64);
  matrix->begin();
  textColor = matrix->Color333(7,0,0);
  x = 64;
}

void loop() {
  server.handleClient();

  matrix->fillScreen(0);
  matrix->setCursor(x, 12);
  matrix->setTextColor(textColor);
  matrix->setTextSize(1);
  matrix->print(text);

  if(--x < -200) x = 64;

  delay(speedScroll);
}
