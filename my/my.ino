// TRIG -> GPIO5, ECHO -> GPIO18, VCC -> 5V, GND -> GND

#include <WiFi.h>
#include <WebServer.h>

const char* SSID = "myminegwe";
const char* PASS = "12345678";
const int TRIG = 5, ECHO = 18;
WebServer server(80);

float jarak() {
  digitalWrite(TRIG, LOW); delayMicroseconds(2);
  digitalWrite(TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  long d = pulseIn(ECHO, HIGH, 30000);
  return d == 0 ? -1 : (d * 0.0343) / 2.0;
}

const char PAGE[] PROGMEM = R"html(
<!DOCTYPE html><html><head><meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
body{background:#1e1e2e;color:#cdd6f4;font-family:sans-serif;display:flex;
flex-direction:column;align-items:center;justify-content:center;height:100vh;margin:0}
#v{font-size:5rem;font-weight:600;color:#cba6f7}
#u{color:#6c7086;font-size:.9rem}
</style></head><body>
<div id="v">--</div><div id="u">cm</div>
<script>
async function tick(){
  try{
    const r=await fetch('/d');const j=await r.json();
    document.getElementById('v').textContent=j.jarak<0?'--':j.jarak.toFixed(1);
    document.getElementById('v').style.color=j.jarak<0?'#f38ba8':'#cba6f7';
  }catch(e){}
}
setInterval(tick,500);tick();
</script></body></html>
)html";

void setup() {
  Serial.begin(19200);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  bool ok = WiFi.softAP(SSID, PASS);
  Serial.println(ok ? "AP started" : "AP FAILED");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", []() { server.send(200, "text/html", PAGE); });
  server.on("/d", []() {
    server.send(200, "application/json", "{\"jarak\":" + String(jarak(), 1) + "}");
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
