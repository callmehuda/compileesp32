#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32-Dino";
const char* password = "dino1234";

#define TOUCH_PIN T0
int touchThreshold = 0;
bool wasTouched = false;
volatile bool jumpFlag = false;
unsigned long lastTouchTime = 0;
const unsigned long debounceMs = 200;

WebServer server(80);

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Dino Run</title>
<style>
  body { background:#f7f7f7; font-family:monospace; text-align:center; margin-top:30px; }
  canvas { background:#fff; border:2px solid #333; touch-action:none; }
  #score { font-size:20px; margin-bottom:10px; }
  #status { color:#555; margin-top:10px; font-size:14px; }
</style>
</head>
<body>
<h2>ESP32 Dino Run</h2>
<div id="score">Score: 0</div>
<canvas id="game" width="600" height="200"></canvas>
<div id="status">Sentuh pin touch sensor (GPIO4) untuk lompat</div>
<script>
const c = document.getElementById('game'), ctx = c.getContext('2d'), groundY = 160;
let dino = { x:50, y:groundY-40, w:30, h:40, vy:0, jumping:false };
const gravity = 1.4, jumpForce = -18;
let obstacles = [], speed = 6, score = 0, gameOver = false, spawnTimer = 0;

function resetGame() {
  Object.assign(dino, { y:groundY-40, vy:0, jumping:false });
  obstacles = []; speed = 6; score = 0; gameOver = false; spawnTimer = 0;
}

function doJump() {
  if (gameOver) return resetGame();
  if (!dino.jumping) { dino.vy = jumpForce; dino.jumping = true; }
}

function update() {
  if (gameOver) return;
  dino.vy += gravity;
  dino.y += dino.vy;
  if (dino.y >= groundY-40) { dino.y = groundY-40; dino.vy = 0; dino.jumping = false; }

  if (--spawnTimer <= 0) {
    const h = 20 + Math.random()*20;
    obstacles.push({ x:c.width, y:groundY-h, w:16, h });
    spawnTimer = 60 + Math.random()*60;
  }
  obstacles.forEach(o => o.x -= speed);
  obstacles = obstacles.filter(o => o.x+o.w > 0);

  for (const o of obstacles) {
    if (dino.x < o.x+o.w && dino.x+dino.w > o.x && dino.y < o.y+o.h && dino.y+dino.h > o.y) gameOver = true;
  }
  score++; speed += 0.002;
  document.getElementById('score').innerText = 'Score: ' + (score/5|0);
}

function draw() {
  ctx.clearRect(0,0,c.width,c.height);
  ctx.strokeStyle = '#333';
  ctx.beginPath(); ctx.moveTo(0,groundY); ctx.lineTo(c.width,groundY); ctx.stroke();
  ctx.fillStyle = '#333'; ctx.fillRect(dino.x,dino.y,dino.w,dino.h);
  ctx.fillStyle = '#2a7'; obstacles.forEach(o => ctx.fillRect(o.x,o.y,o.w,o.h));
  if (gameOver) { ctx.fillStyle = '#c33'; ctx.font = '18px monospace'; ctx.fillText('GAME OVER - sentuh untuk ulang', 150, 90); }
}

function loop() { update(); draw(); requestAnimationFrame(loop); }
resetGame(); loop();

document.addEventListener('keydown', e => { if (e.code === 'Space' || e.code === 'ArrowUp') doJump(); });
c.addEventListener('mousedown', doJump);
c.addEventListener('touchstart', doJump);
setInterval(() => fetch('/jump').then(r => r.text()).then(t => t === '1' && doJump()).catch(()=>{}), 60);
</script>
</body>
</html>
)rawliteral";

void handleRoot() { server.send_P(200, "text/html", htmlPage); }

void handleJump() {
  server.send(200, "text/plain", jumpFlag ? "1" : "0");
  jumpFlag = false;
}

void calibrateTouch() {
  long sum = 0;
  for (int i = 0; i < 20; i++) { sum += touchRead(TOUCH_PIN); delay(10); }
  touchThreshold = (sum / 20) * 0.65;
}

void setup() {
  Serial.begin(19200);
  calibrateTouch();

  WiFi.softAP(ssid, password);
  Serial.print("AP aktif. Buka: http://");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/jump", handleJump);
  server.begin();
}

void loop() {
  server.handleClient();
  bool isTouched = touchRead(TOUCH_PIN) < touchThreshold;
  if (isTouched && !wasTouched && millis() - lastTouchTime > debounceMs) {
    jumpFlag = true;
    lastTouchTime = millis();
  }
  wasTouched = isTouched;
}
