#include "touch.h"

using namespace daisy;

namespace Touch {

bool Controller::Init(I2CHandle& i2c, DaisySeed& seed)
{
    i2c_ = &i2c;

    reset_pin_.Init(seed.GetPin(Pins::TOUCH_RST), GPIO::Mode::OUTPUT);
    reset_pin_.Write(true);
    change_pin_.Init(seed.GetPin(Pins::TOUCH_CHG), GPIO::Mode::INPUT);

    reset_pin_.Write(false);
    System::Delay(10);
    reset_pin_.Write(true);
    System::Delay(200);

    if(ReadReg(Register::CHIP_ID) != CHIP_ID_VALUE)
        return false;

    for(uint8_t k = 0; k < 8; k++)
        SetThreshold(k, 10);
    for(uint8_t k = 8; k < 12; k++)
        SetThreshold(k, 0);

    Calibrate();
    return true;
}

TouchState Controller::Read()
{
    TouchState state = {};

    uint8_t det    = ReadReg(Register::DETECTION_STATUS);
    uint8_t keys_l = ReadReg(Register::KEY_STATUS_1);
    uint8_t keys_h = ReadReg(Register::KEY_STATUS_2);

    state.keys      = (uint16_t)keys_l | ((uint16_t)(keys_h & 0x0F) << 8);
    state.any_touch = (det & 0x01) != 0;
    state.overflow  = (det & 0x10) != 0;
    state.slider_pos = ReadReg(Register::SLIDER_POSITION);

    return state;
}

void Controller::Calibrate()
{
    WriteReg(Register::CALIBRATE, 0x01);
    System::Delay(2000);
}

void Controller::SetThreshold(uint8_t key, uint8_t threshold)
{
    if(key > 11)
        return;
    WriteReg(Register::KEY0_THRESH + key, threshold);
}

bool Controller::WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return i2c_->TransmitBlocking(I2CAddr::AT42QT2120, buf, 2, 100)
           == I2CHandle::Result::OK;
}

uint8_t Controller::ReadReg(uint8_t reg)
{
    uint8_t val = 0;
    i2c_->TransmitBlocking(I2CAddr::AT42QT2120, &reg, 1, 100);
    i2c_->ReceiveBlocking(I2CAddr::AT42QT2120, &val, 1, 100);
    return val;
}

}  // namespace Touch
