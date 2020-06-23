// ------------------------------------------------------------
// Copyright 2020-present Sergey Kovalevich <inndie@gmail.com>
// ------------------------------------------------------------

#include <array>

#include "display.h"

#include <BleKeyboard.h>
#include <M5Stack.h>

using namespace app;

BleKeyboard keyboard("Evil Keyboard");

// C++17 deduction guides doesn't work =(
constexpr auto strings =
    std::array<char const*, 7>({"Hello", "Fuck you!", "Have a nice day!", "Im done", "GG", "Have fun!", "Dude!"});
std::size_t currentStringIndex = 0;

inline void updateStringIndex(int offset = 0) noexcept {
  currentStringIndex = (currentStringIndex + strings.size() + offset) % strings.size();
}

void showBluetoothConnectedState() {
  static bool keyboardConnectedFlag = true;

  if (keyboardConnectedFlag == keyboard.isConnected()) {
    return;
  }
  keyboardConnectedFlag = keyboard.isConnected();

  print<Row<0>, DarkGrey, Black>("Bluetooh: %s", keyboardConnectedFlag ? "connected" : "disconnected");
}

char const* getCurrentString() noexcept {
  return strings[currentStringIndex];
}

void showCurrentString() {
  static char const* currentString = nullptr;
  if (currentString == getCurrentString()) {
    return;
  }
  currentString = getCurrentString();
  print<Row<4>, Orange, Black>("  %s", currentString);
}

void showBatteryLevel() {
  static int batteryLevel = -1;

  int const value = M5.Power.getBatteryLevel();
  if (value == batteryLevel) {
    return;
  }
  batteryLevel = value;
  print<Row<1>, DarkGrey, Black>("Battery: %d%%", batteryLevel);
}

void setup() {
  M5.begin();
  M5.Power.begin();
  M5.Power.setWakeupButton(BUTTON_B_PIN);

  keyboard.begin();

  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextSize(2);

  print<Row<3>, White, Black>("Phrase:");
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    updateStringIndex(-1);
  }

  if (M5.BtnC.wasPressed()) {
    updateStringIndex(1);
  }

  if (M5.BtnB.pressedFor(3000)) {
    M5.Power.deepSleep(0);
  } else if (M5.BtnB.wasReleased()) {
    keyboard.print(getCurrentString());
    keyboard.write(KEY_RETURN);
  }

  showBluetoothConnectedState();
  showBatteryLevel();
  showCurrentString();

  uint32_t const now = millis();
  uint32_t const lastChange = std::max({M5.BtnA.lastChange(), M5.BtnB.lastChange(), M5.BtnC.lastChange()});
  constexpr uint32_t idleTimeout = 5 * 60 * 1000;
  if (now - lastChange > idleTimeout) {
    M5.Power.deepSleep(0);
  }
}
