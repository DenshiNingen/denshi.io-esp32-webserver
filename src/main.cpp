#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "";
const char* password = "";

AsyncWebServer server(80);

// NeoPixel setup
#define PIXEL_PIN 38
#define NUM_PIXELS 1
Adafruit_NeoPixel pixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Function to blink red for 1s
void blinkRed() {
  pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red
  pixel.show();
  delay(1000);
  pixel.clear();
  pixel.show();
}

void notFound(AsyncWebServerRequest *request) {
  if (request->method() == HTTP_OPTIONS) {
    request->send(200);
  } else {
    request->send(404, "application/json", "{\"message\":\"Not found\"}");
  }
}

void setup() {
  Serial.begin(115200);

  // Init NeoPixel
  pixel.begin();
  pixel.clear();
  pixel.show();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Mount LittleFS
  if (!LittleFS.begin(true)) {
    Serial.println("Failed to mount LittleFS");
    return;
  }
  uint32_t totalBytes = LittleFS.totalBytes();
  uint32_t usedBytes = LittleFS.usedBytes();
  Serial.printf("Total: %d, Used: %d\n", totalBytes, usedBytes);

  // CORS headers for dev
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

  // Serve static files
  server.on("/", HTTP_GET, handleHomePage);
  server.serveStatic("/static/", LittleFS, "/");

  // Blink when homepage is requested
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    blinkRed();
    request->send(LittleFS, "/index.html", "text/html");
  });

  server.onNotFound(notFound);

  server.begin();
}

void loop() {
  // Nothing to do here as we're using an async server
}
