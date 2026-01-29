#include <WiFi.h>
#include <WebServer.h>

// --- KONFIGURASI WIFI ---
const char* ssid = "My";
const char* password = "llllllll";

WebServer server(80);
const int ledPin = 2; // LED bawaan ESP32

// Tampilan Web dengan 1 Tombol Hold
String getHTML() {
  String html = "<!DOCTYPE html><html>";
  html += "<head><meta name='viewport' content='width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no'>";
  html += "<style>";
  html += "body { display: flex; justify-content: center; align-items: center; height: 100vh; margin: 0; background-color: #121212; font-family: sans-serif; }";
  html += ".hold-btn { width: 200px; height: 200px; border-radius: 50%; border: none; background: radial-gradient(#ff3333, #800000); color: white; font-weight: bold; font-size: 24px; cursor: pointer; box-shadow: 0 0 20px rgba(255,0,0,0.5); user-select: none; -webkit-tap-highlight-color: transparent; }";
  html += ".hold-btn:active { background: radial-gradient(#ff6666, #b30000); transform: scale(0.95); box-shadow: 0 0 10px rgba(255,0,0,0.8); }";
  html += "</style></head><body>";
  
  html += "<button class='hold-btn' id='btn'>Idk</button>";

  html += "<script>";
  html += "const btn = document.getElementById('btn');";
  
  // Fungsi kirim perintah ke ESP32
  html += "function sendRequest(state) { fetch('/led?status=' + state); }";

  // Event untuk Mouse (Laptop/PC)
  html += "btn.addEventListener('mousedown', () => sendRequest('on'));";
  html += "btn.addEventListener('mouseup', () => sendRequest('off'));";

  // Event untuk Touch (HP/Tablet)
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

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nTerhubung!");
  Serial.print("Buka alamat ini di browser: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/led", handleLED);
  server.begin();
}

void loop() {
  server.handleClient();
}
