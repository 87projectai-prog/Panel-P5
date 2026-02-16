#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

// ===== PIN MAPPING =====
#define B1 4
#define B2 7
#define A 8
#define B 9
#define C 10
#define D 20
#define CLK 1
#define LAT 21
#define OE 0

// ===== PANEL SIZE =====
#define WIDTH 64
#define HEIGHT 32

// ===== WEB SERVER =====
WebServer server(80);

// ===== TEXT =====
String text = "87PROJECT";
int scrollSpeed = 50;
int scrollX = WIDTH;

// ===== BIT-BANG FUNCTION =====
void setRow(int row) {
  // Set row address
  digitalWrite(A, row & 0x01);
  digitalWrite(B, row & 0x02);
  digitalWrite(C, row & 0x04);
  digitalWrite(D, row & 0x08);
}

void clockPulse() {
  digitalWrite(CLK, HIGH);
  delayMicroseconds(1);
  digitalWrite(CLK, LOW);
}

void latchRow() {
  digitalWrite(LAT, HIGH);
  delayMicroseconds(1);
  digitalWrite(LAT, LOW);
}

// ===== WEB HANDLER =====
void handleRoot() {
  String page = "<html><body style='background:black;color:blue;text-align:center'>";
  page += "<h2>P5 CONTROL - BIRU</h2>";
  page += "<form action='/set'>";
  page += "Text:<br><input name='msg'><br><br>";
  page += "Scroll Speed:<br><input name='spd' type='number'><br><br>";
  page += "<input type='submit' value='Update'>";
  page += "</form></body></html>";
  server.send(200, "text/html", page);
}

void handleSet() {
  if(server.hasArg("msg")) text = server.arg("msg");
  if(server.hasArg("spd")) scrollSpeed = server.arg("spd").toInt();
  scrollX = WIDTH;
  server.sendHeader("Location","/");
  server.send(303);
}

// ===== SETUP =====
void setup() {
  Serial.begin(115200);

  // GPIO OUTPUT
  pinMode(B1, OUTPUT);
  pinMode(B2, OUTPUT);
  pinMode(A, OUTPUT);
  pinMode(B, OUTPUT);
  pinMode(C, OUTPUT);
  pinMode(D, OUTPUT);
  pinMode(CLK, OUTPUT);
  pinMode(LAT, OUTPUT);
  pinMode(OE, OUTPUT);

  digitalWrite(OE, HIGH);
  digitalWrite(CLK, LOW);
  digitalWrite(LAT, LOW);

  // START WIFI AP
  WiFi.softAP("P5-CONTROL", "12345678");
  Serial.println("WiFi AP Started");
  Serial.println(WiFi.softAPIP());

  // WEB ROUTES
  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

// ===== LOOP =====
void loop() {
  server.handleClient();

  // ===== CLEAR PANEL =====
  digitalWrite(OE, HIGH); // matikan dulu
  for(int row=0; row<HEIGHT/2; row++) {
    setRow(row);
    for(int col=0; col<WIDTH; col++) {
      // Set B1/B2 HIGH untuk warna biru jika teks nyala
      // Bit sederhana: scroll text manual (dummy)
      if(col >= scrollX && col < scrollX + text.length()*6) {
        digitalWrite(B1, HIGH);
        digitalWrite(B2, LOW);
      } else {
        digitalWrite(B1, LOW);
        digitalWrite(B2, LOW);
      }
      clockPulse();
    }
    latchRow();
  }
  digitalWrite(OE, LOW); // nyalakan panel

  // ===== SCROLL TEXT =====
  scrollX--;
  if(scrollX < -text.length()*6) scrollX = WIDTH;

  delay(scrollSpeed);
}
