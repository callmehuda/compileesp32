#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

// Konfigurasi Nama WiFi dan Password yang akan dibuat oleh ESP32
const char* ssid_ap = "My";
const char* password_ap = "12345678"; // Minimal 8 karakter

WebServer server(80);
unsigned long previousMillis = 0;
const long interval = 1000; 
bool ledState = LOW;

const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
  <head><meta name="viewport" content="width=device-width, initial-scale=1"></head>
  <body>
    <h3>ESP32 Access Point Update</h3>
    <form method="POST" action="/update" enctype="multipart/form-data">
      <input type="file" name="update" accept=".bin" />
      <input type="submit" value="Upload & Update" />
    </form>
  </body>
</html>
)HTML";

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);

  // Mengubah mode menjadi Access Point
  WiFi.softAP(ssid_ap, password_ap);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP); // Biasanya 192.168.4.1

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", INDEX_HTML);
  });

  server.on("/update", HTTP_POST, []() {
    bool ok = !Update.hasError();
    server.send(200, "text/plain", ok ? "Update Berhasil! Restarting..." : "Update Gagal!");
    delay(500);
    if (ok) ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) {
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });
  
  server.begin();
}

void loop() {
  server.handleClient();

  // Blink LED tanpa delay agar web server tetap responsif
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(2, ledState);
  }
}
