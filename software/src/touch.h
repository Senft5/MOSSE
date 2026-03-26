#pragma once
#include "daisy_seed.h"
#include "pin_defs.h"

namespace Touch {

enum Register : uint8_t {
    CHIP_ID          = 0x00,
    FIRMWARE_VER     = 0x01,
    DETECTION_STATUS = 0x02,
    KEY_STATUS_1     = 0x03,
    KEY_STATUS_2     = 0x04,
    SLIDER_POSITION  = 0x05,
    CALIBRATE        = 0x06,
    RESET            = 0x07,
    LP_MODE          = 0x08,
    TTD              = 0x09,
    ATD              = 0x0A,
    DI               = 0x0B,
    TOUCH_RECAL      = 0x0C,
    DRIFT_HOLD       = 0x0D,
    SLIDER_OPTION    = 0x0E,
    CHARGE_TIME      = 0x0F,
    KEY0_THRESH      = 0x10,
    KEY0_CONTROL     = 0x1C,
    KEY0_SIGNAL_MSB  = 0x28,
    KEY0_REF_MSB     = 0x40,
};

constexpr uint8_t CHIP_ID_VALUE = 0x3E;

struct TouchState {
    uint16_t keys;
    uint8_t  slider_pos;
    bool     any_touch;
    bool     overflow;
};

class Controller {
public:
    bool Init(daisy::I2CHandle& i2c, daisy::DaisySeed& seed);
    TouchState Read();
    void Calibrate();
    void SetThreshold(uint8_t key, uint8_t threshold);

private:
    daisy::I2CHandle* i2c_;
    daisy::GPIO reset_pin_;
    daisy::GPIO change_pin_;

    bool WriteReg(uint8_t reg, uint8_t val);
    uint8_t ReadReg(uint8_t reg);
};

}  // namespace Touch
