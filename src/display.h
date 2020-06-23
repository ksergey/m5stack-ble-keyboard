// ------------------------------------------------------------
// Copyright 2020-present Sergey Kovalevich <inndie@gmail.com>
// ------------------------------------------------------------

#pragma once

#include <M5Stack.h>

namespace app {

template<std::uint16_t C>
struct Color {
  static constexpr auto value = C;
};

using Black = Color<TFT_BLACK>;
using White = Color<TFT_WHITE>;
using Orange = Color<TFT_ORANGE>;
using DarkGrey= Color<TFT_DARKGREY>;

template<std::size_t N>
struct Row {
  static constexpr auto value = N;
};

template<typename Row, typename Fg, typename Bg, typename... Ts>
inline void print(Ts&&... args) noexcept {
  std::int16_t const width = M5.Lcd.width();
  std::int16_t const height = M5.Lcd.textsize * 10;
  std::int16_t const x = 0;
  std::int16_t const y = height * Row::value;

  M5.Lcd.fillRect(x, y, width, height, Bg::value);
  M5.Lcd.setCursor(x, y);
  M5.Lcd.setTextColor(Fg::value, Bg::value);
  M5.Lcd.printf(std::forward<Ts>(args)...);
}

} // namespace app
