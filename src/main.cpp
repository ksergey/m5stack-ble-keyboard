// ------------------------------------------------------------
// Copyright 2020-present Sergey Kovalevich <inndie@gmail.com>
// ------------------------------------------------------------

#include <array>
#include <type_traits>

#include <BleKeyboard.h>
#include <M5Stack.h>

BleKeyboard keyboard("Evil Keyboard");

template<std::size_t N>
struct Row {
  static constexpr std::size_t value = N;
};

template<std::uint16_t C>
struct Color {
  static constexpr std::uint16_t value = C;
};

template<typename Row, typename Fg = Color<TFT_WHITE>, typename Bg = Color<TFT_BLACK>, typename... Ts>
void updateDisplay(Ts&&... args) {
  constexpr std::size_t width = 320;
  constexpr std::size_t x = 0;
  std::size_t const height = std::size_t(M5.Lcd.textsize) * 10;
  std::size_t const y = height * Row::value;

  M5.Lcd.fillRect(x, y, width, height, Bg::value);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.setTextColor(Fg::value, Bg::value);
  M5.Lcd.printf(std::forward<Ts>(args)...);
}

// C++17 deduction guides doesn't work =(
constexpr auto strings =
    std::array<char const*, 7>({"Hello", "Fuck you!", "Have a nice day!", "Im done", "GG", "Have fun!", "Dude!"});
std::size_t currentStringIndex = 0;

inline void updateStringIndex(int offset = 0) noexcept {
  currentStringIndex = (currentStringIndex + strings.size() + offset) % strings.size();
}

std::int8_t batteryLevel = 0;

int8_t getBatteryLevel(bool force = false) noexcept {
  static std::int8_t value = -1;
  static unsigned long prevMillis = 0;

  auto newMillis = ::millis();
  if (newMillis - prevMillis >= 5000 || force) {
    prevMillis = newMillis;
    value = M5.Power.getBatteryLevel();
  }

  return value;
}

void showConnectedState() {
  static bool keyboardConnectedFlag = true;

  if (keyboardConnectedFlag == keyboard.isConnected()) {
    return;
  }
  keyboardConnectedFlag = keyboard.isConnected();

  updateDisplay<Row<0>>("Bluetooh: %s", keyboardConnectedFlag ? "Connected" : "Disconnected");
}

void showBatteryLevel() {
  static std::int8_t batteryLevel = -1;

  if (batteryLevel == getBatteryLevel()) {
    return;
  }
  batteryLevel = getBatteryLevel();

  updateDisplay<Row<1>>("Battery: %d%%", int(batteryLevel));
}

void keyboardUpdateBatteryLevel() {
  if (keyboard.batteryLevel != getBatteryLevel()) {
    keyboard.setBatteryLevel(getBatteryLevel());
  }
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
  updateDisplay<Row<4>, Color<TFT_ORANGE>>("  %s", currentString);
}

void setup() {
  M5.begin();
  M5.Power.begin();

  keyboard.begin();

  M5.Lcd.clear(BLACK);
  M5.Lcd.setTextSize(2);

  updateDisplay<Row<3>>("Phrase:");

  getBatteryLevel(true);
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
      keyboard.write(KEY_RETURN);
      keyboard.print(getCurrentString());
      keyboard.write(KEY_RETURN);
    }
  }

  showConnectedState();
  showBatteryLevel();
  showCurrentString();
  keyboardUpdateBatteryLevel();
}
