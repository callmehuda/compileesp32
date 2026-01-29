#include <WiFi.h>
#include <WebServer.h>

// Nama WiFi dan Password yang akan dipancarkan ESP32
const char* ssid = "ESP32-Tombol";
const char* password = "password123";

WebServer server(80);
const int ledPin = 2; // LED bawaan DevKitC V4

String getHTML() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
  html += "<style>";
  html += "body { display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #121212; font-family: sans-serif; }";
  html += ".hold-btn { width: 220px; height: 220px; border-radius: 50%; border: 8px solid #333; background: radial-gradient(#ff3333, #800000); color: white; font-weight: bold; font-size: 28px; cursor: pointer; box-shadow: 0 0 30px rgba(255,0,0,0.4); user-select: none; -webkit-tap-highlight-color: transparent; }";
  html += ".hold-btn:active { background: radial-gradient(#ff6666, #b30000); transform: scale(0.92); box-shadow: 0 0 15px rgba(255,0,0,0.7); border-color: #555; }";
  html += "</style></head><body>";
  
  html += "<button class='hold-btn' id='btn'>TAHAN</button>";

  html += "<script>";
  html += "const btn = document.getElementById('btn');";
  html += "function sendRequest(state) { fetch('/led?status=' + state); }";

  // PC/Laptop
  btn.addEventListener('mousedown', () => sendRequest('on'));
  btn.addEventListener('mouseup', () => sendRequest('off'));

  // HP/Tablet
  btn.addEventListener('touchstart', (e) => { e.preventDefault(); sendRequest('on'); });
  btn.addEventListener('touchend', (e) => { e.preventDefault(); sendRequest('off'); });
  html += "</script></body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
}

void handleLED() {
  if (server.hasArg("status")) {
    String status = server.arg("status");
    digitalWrite(ledPin, (status == "on") ? HIGH : LOW);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Set sebagai Access Point
  WiFi.softAP(ssid, password);

  Serial.println("WiFi AP Berhasil Dibuat!");
  Serial.print("Nama WiFi: ");
  Serial.println(ssid);
  Serial.print("Buka browser ke IP: ");
  Serial.println(WiFi.softAPIP()); // Default biasanya 192.168.4.1

  server.on("/", handleRoot);
  server.on("/led", handleLED);
  server.begin();
}

void loop() {
  server.handleClient();
}
