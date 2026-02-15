#include <WiFi.h>
#include <WebServer.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

WebServer server(80);

String text = "87PROJECT";
int scrollSpeed = 30;
int x;

MatrixPanel_I2S_DMA *dma_display = nullptr;

void handleRoot() {
  String page = "<html><body style='background:black;color:lime;text-align:center'>";
  page += "<h2>P5 CONTROL</h2>";
  page += "<form action='/set'>";
  page += "Text:<br><input name='msg'><br><br>";
  page += "Speed:<br><input name='spd' type='number'><br><br>";
  page += "<input type='submit' value='Update'>";
  page += "</form></body></html>";
  server.send(200, "text/html", page);
}

void handleSet() {
  if(server.hasArg("msg")) text = server.arg("msg");
  if(server.hasArg("spd")) scrollSpeed = server.arg("spd").toInt();
  x = dma_display->width();
  server.sendHeader("Location","/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);
  WiFi.softAP("P5-CONTROL", "12345678");

  HUB75_I2S_CFG mxconfig(
    64,   // width
    32,   // height
    1     // number of panels
  );

  dma_display = new MatrixPanel_I2S_DMA(mxconfig);
  dma_display->begin();
  dma_display->setBrightness8(80);

  x = dma_display->width();

  server.on("/", handleRoot);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();

  dma_display->clearScreen();
  dma_display->setTextSize(1);
  dma_display->setTextColor(dma_display->color565(255,0,0));
  dma_display->setCursor(x, 12);
  dma_display->print(text);

  if(--x < -200) x = dma_display->width();

  delay(scrollSpeed);
}
