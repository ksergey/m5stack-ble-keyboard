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

void showConnectedState() {
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
  print<Row<3>, Orange, Black>("  %s", currentString);
}

void setup() {
  M5.begin();

  keyboard.begin();

  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextSize(2);

  print<Row<2>, White, Black>("Phrase:");
}

void loop() {
  M5.update();

  if (M5.BtnA.wasPressed()) {
    updateStringIndex(-1);
  }

  if (M5.BtnC.wasPressed()) {
    updateStringIndex(1);
  }

  if (M5.BtnB.wasPressed()) {
    if (keyboard.isConnected()) {
      keyboard.print(getCurrentString());
      keyboard.write(KEY_RETURN);
    }
  }

  showConnectedState();
  showCurrentString();
}
