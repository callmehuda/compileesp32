#include <BleKeyboard.h>

BleKeyboard bleKeyboard("ESP32 WASD", "DIY", 100);

constexpr uint8_t touchPins[4] = {T0, T2, T3, T4};
constexpr char keys[4] = {'w', 'a', 's', 'd'};
constexpr int TOUCH_MARGIN = 8;
constexpr unsigned long PRESS_DEBOUNCE_US = 2000;
constexpr unsigned long RELEASE_DEBOUNCE_US = 20000;
constexpr int CALIBRATION_SAMPLES = 32;

int baseline[4];
bool rawState[4] = {};
bool pressed[4] = {};
unsigned long lastRawChange[4] = {};

void calibrate() {
  for (int i = 0; i < 4; i++) {
    long sum = 0;
    for (int s = 0; s < CALIBRATION_SAMPLES; s++) sum += touchRead(touchPins[i]);
    baseline[i] = sum / CALIBRATION_SAMPLES;
  }
}

void setup() {
  calibrate();
  bleKeyboard.begin();
}

void loop() {
  if (!bleKeyboard.isConnected()) return;

  unsigned long now = micros();

  for (int i = 0; i < 4; i++) {
    bool touched = touchRead(touchPins[i]) < baseline[i] - TOUCH_MARGIN;

    if (touched != rawState[i]) {
      rawState[i] = touched;
      lastRawChange[i] = now;
    }

    unsigned long debounce = rawState[i] ? PRESS_DEBOUNCE_US : RELEASE_DEBOUNCE_US;
    if (rawState[i] != pressed[i] && now - lastRawChange[i] > debounce) {
      pressed[i] = rawState[i];
      pressed[i] ? bleKeyboard.press(keys[i]) : bleKeyboard.release(keys[i]);
    }
  }
}
