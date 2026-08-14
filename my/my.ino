#include <BleKeyboard.h>

BleKeyboard bleKeyboard("Nigger", "ESP32", 100);

constexpr uint8_t PIN_COUNT = 4;
constexpr uint8_t TOUCH_PINS[PIN_COUNT] = {T7, T6, T5, T4};
constexpr uint8_t KEY_MAP[PIN_COUNT] = {'d', 'f', 'j', 'k'};
constexpr uint16_t CALIB_SAMPLES = 200;
constexpr float TOUCH_RATIO = 0.85f;
constexpr uint8_t CONFIRM_COUNT = 2;

uint16_t baseline[PIN_COUNT];
uint16_t threshold[PIN_COUNT];
bool pressed[PIN_COUNT] = {false};
uint8_t confirmCounter[PIN_COUNT] = {0};

void calibrate() {
  uint32_t sums[PIN_COUNT] = {0};
  for (uint16_t i = 0; i < CALIB_SAMPLES; i++) {
    for (uint8_t p = 0; p < PIN_COUNT; p++) {
      sums[p] += touchRead(TOUCH_PINS[p]);
    }
  }
  for (uint8_t p = 0; p < PIN_COUNT; p++) {
    baseline[p] = sums[p] / CALIB_SAMPLES;
    threshold[p] = baseline[p] * TOUCH_RATIO;
  }
}

void setup() {
  bleKeyboard.begin();
  calibrate();
}

void loop() {
  if (!bleKeyboard.isConnected()) {
    delay(50);
    return;
  }

  for (uint8_t p = 0; p < PIN_COUNT; p++) {
    uint16_t val = touchRead(TOUCH_PINS[p]);
    bool touching = val < threshold[p];

    if (touching != pressed[p]) {
      confirmCounter[p]++;
      if (confirmCounter[p] >= CONFIRM_COUNT) {
        pressed[p] = touching;
        confirmCounter[p] = 0;
        if (pressed[p]) {
          bleKeyboard.press(KEY_MAP[p]);
        } else {
          bleKeyboard.release(KEY_MAP[p]);
        }
      }
    } else {
      confirmCounter[p] = 0;
    }
  }
}
