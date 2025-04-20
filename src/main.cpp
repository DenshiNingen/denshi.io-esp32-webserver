#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>

// Wi-Fi credentials
const char* ssid = "";
const char* password = "";

// Async web server
AsyncWebServer server(80);

// NeoPixel setup
#define PIXEL_PIN 38
#define NUM_PIXELS 1
Adafruit_NeoPixel pixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Blinking state
bool isBlinking = false;
unsigned long blinkStartTime = 0;

// Start blinking red
void blinkRed() {
  Serial.println("[BLINK] Starting red blink...");
  pixel.setPixelColor(0, pixel.Color(255, 0, 0)); // Red
  pixel.show();
  isBlinking = true;
  blinkStartTime = millis();
}

// 404 or OPTIONS handler
void notFound(AsyncWebServerRequest *request) {
  Serial.printf("[404] Not found: %s\n", request->url().c_str());
  if (request->method() == HTTP_OPTIONS) {
    request->send(200);
  } else {
    request->send(404, "application/json", "{\"message\":\"Not found\"}");
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n[SETUP] Booting...");

  // Initialize NeoPixel
  pixel.begin();
  pixel.clear();
  pixel.show();
  Serial.println("[SETUP] NeoPixel initialized");

  // Connect to Wi-Fi
  Serial.print("[WiFi] Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Connected!");
  Serial.print("[WiFi] IP Address: ");
  Serial.println(WiFi.localIP());

  // Mount filesystem
  Serial.println("[FS] Mounting LittleFS...");
  if (!LittleFS.begin(true)) {
    Serial.println("[FS] Failed to mount LittleFS!");
    return;
  }
  Serial.printf("[FS] Mounted! Total: %d bytes, Used: %d bytes\n", LittleFS.totalBytes(), LittleFS.usedBytes());

  // CORS headers
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
  Serial.println("[CORS] Headers set");

  // Serve static files properly
  server.serveStatic("/static/", LittleFS, "/static/");
  Serial.println("[SERVER] Static file serving configured");

  // Blink on every request
  // Special handler for "/"
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("[HTTP] GET / (root)");
    blinkRed();
    request->send(LittleFS, "/index.html", "text/html");
  });
  Serial.println("[SERVER] Global request handler added");

  // 404 for unknown paths
  server.onNotFound(notFound);

  // Start server
  server.begin();
  Serial.println("[SERVER] Server started!");
}

void loop() {
  if (isBlinking && millis() - blinkStartTime >= 1000) {
    Serial.println("[BLINK] Ending red blink...");
    pixel.clear();
    pixel.show();
    isBlinking = false;
  }
}
