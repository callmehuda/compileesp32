#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>

const char* ssid = "TP-Link";
const char* password = "";

WebServer server(80);

const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
  <body>
    <h3>ESP32 Web Update</h3>
    <form method="POST" action="/update" enctype="multipart/form-data">
      <input type="file" name="update" />
      <input type="submit" value="Update" />
    </form>
  </body>
</html>
)HTML";

void setup() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }

  Serial.begin(115200);
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", INDEX_HTML);
  });
  server.on("/update", HTTP_POST, []() {
    bool ok = !Update.hasError();
    server.send(200, "text/plain", ok ? "OK" : "FAIL");
    delay(100);
    if (ok) ESP.restart();
  }, []() {
    HTTPUpload& upload = server.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Update.begin(UPDATE_SIZE_UNKNOWN);
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      Update.write(upload.buf, upload.currentSize);
    } else if (upload.status == UPLOAD_FILE_END) {
      Update.end(true);
    }
  });
  
  server.begin();
pinMode(2, OUTPUT);

}

void loop() {
  server.handleClient();

  digitalWrite(2, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(2, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second

}