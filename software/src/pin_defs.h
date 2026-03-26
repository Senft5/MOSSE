#pragma once
#include "daisy_seed.h"

// daisy pin idx = physical - 1 (pin 9 -> 8, etc)

namespace Pins {

using namespace daisy;

constexpr uint8_t DISP_SCK   = 8;
constexpr uint8_t DISP_MOSI  = 10;
constexpr uint8_t DISP_CS    = 7;
constexpr uint8_t DISP_DC    = 13;
constexpr uint8_t DISP_RST   = 14;
constexpr uint8_t DISP_PWM   = 20; // BL, inverted at q1

constexpr uint8_t I2C_SCL = 11;
constexpr uint8_t I2C_SDA = 12;

constexpr uint8_t SD_CD = 15;

constexpr uint8_t TOUCH_CHG = 16;
constexpr uint8_t TOUCH_RST = 17;

constexpr uint8_t GPIO_ON  = 18;
constexpr uint8_t GPIO_OFF = 19;

constexpr uint8_t BTN_N = 21;
constexpr uint8_t BTN_S = 22;
constexpr uint8_t BTN_E = 23;
constexpr uint8_t BTN_W = 24;

constexpr uint8_t FUSB_INT = 25;

}  // namespace Pins

namespace I2CAddr {

constexpr uint8_t AT42QT2120 = 0x1C;
constexpr uint8_t DRV2605L   = 0x5A;
constexpr uint8_t FUSB302    = 0x22;

}  // namespace I2CAddr
