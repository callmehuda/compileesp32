#include <WiFi.h>
#include <WebServer.h>

// --- KONFIGURASI WIFI (Mode Access Point) ---
const char* ssid = "ESP32-Tombol";
const char* password = "password123";

WebServer server(80);
const int ledPin = 2; // LED bawaan ESP32 DevKitC V4

// Tampilan Web dengan 1 Tombol Hold
String getHTML() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
  html += "<style>";
  html += "body { display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #121212; font-family: sans-serif; }";
  html += ".hold-btn { width: 220px; height: 220px; border-radius: 50%; border: 6px solid #444; background: radial-gradient(#ff3333, #800000); color: white; font-weight: bold; font-size: 24px; cursor: pointer; box-shadow: 0 0 20px rgba(255,0,0,0.5); user-select: none; -webkit-tap-highlight-color: transparent; transition: 0.1s; }";
  html += ".hold-btn:active { background: radial-gradient(#ff6666, #b30000); transform: scale(0.92); box-shadow: 0 0 10px rgba(255,0,0,0.8); }";
  html += "h2 { position: absolute; top: 20px; color: #555; }";
  html += "</style></head><body>";
  
  html += "<h2>HOLD TO LIGHT</h2>";
  html += "<button class='hold-btn' id='btn'>TAHAN</button>";

  html += "<script>";
  html += "const btn = document.getElementById('btn');";
  
  // Fungsi kirim perintah ke ESP32
  html += "function sendRequest(state) { fetch('/led?status=' + state); }";

  // Event untuk Mouse (PC)
  html += "btn.addEventListener('mousedown', () => sendRequest('on'));";
  html += "btn.addEventListener('mouseup', () => sendRequest('off'));";

  // Event untuk Touch (HP) - e.preventDefault untuk mencegah zoom/delay
  html += "btn.addEventListener('touchstart', (e) => { e.preventDefault(); sendRequest('on'); });";
  html += "btn.addEventListener('touchend', (e) => { e.preventDefault(); sendRequest('off'); });";
  
  html += "</script></body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", getHTML());
}

void handleLED() {
  if (server.hasArg("status")) {
    String status = server.arg("status");
    if (status == "on") {
      digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    }
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Mengaktifkan mode Access Point
  WiFi.softAP(ssid, password);

  Serial.println("\n--- ESP32 WebServer Ready ---");
  Serial.print("WiFi Name: "); Serial.println(ssid);
  Serial.print("IP Address: "); Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/led", handleLED);
  server.begin();
}

void loop() {
  server.handleClient();
}
