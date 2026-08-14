#include <BleKeyboard.h>

BleKeyboard bleKeyboard("ESP32 WASD", "DIY", 100);

constexpr uint8_t touchPins[4] = {T0, T2, T3, T4};
constexpr char keys[4] = {'w', 'a', 's', 'd'};
constexpr int THRESHOLD = 30;
constexpr unsigned long DEBOUNCE_MS = 150;

bool pressed[4] = {false, false, false, false};
unsigned long lastChange[4] = {0, 0, 0, 0};

void setup() {
  bleKeyboard.begin();
}

void loop() {
  if (!bleKeyboard.isConnected()) return;

  unsigned long now = millis();

  for (int i = 0; i < 4; i++) {
    bool touched = touchRead(touchPins[i]) < THRESHOLD;

    if (touched != pressed[i] && now - lastChange[i] > DEBOUNCE_MS) {
      pressed[i] = touched;
      lastChange[i] = now;
      touched ? bleKeyboard.press(keys[i]) : bleKeyboard.release(keys[i]);
    }
  }
}
