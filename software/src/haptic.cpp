#include "haptic.h"

using namespace daisy;

namespace Haptic {

bool Driver::Init(I2CHandle& i2c, bool use_lra)
{
    i2c_     = &i2c;
    use_lra_ = use_lra;

    WriteReg(Register::MODE, Mode::INTERNAL_TRIGGER);

    uint8_t st  = ReadReg(Register::STATUS);
    uint8_t did = (st >> 5) & 0x07;
    if(did == 0)
        return false;

    if(use_lra)
        WriteReg(Register::LIBRARY_SEL, Library::LIB_LRA);
    else
        WriteReg(Register::LIBRARY_SEL, Library::LIB_TS2200_A);

    if(use_lra)
    {
        WriteReg(Register::FEEDBACK_CTRL, 0xA4);
        WriteReg(Register::CONTROL3, 0xA0);
    }
    else
    {
        WriteReg(Register::FEEDBACK_CTRL, 0x24);
        WriteReg(Register::CONTROL3, 0xA1);
    }

    return true;
}

void Driver::PlayEffect(uint8_t effect)
{
    WriteReg(Register::MODE, Mode::INTERNAL_TRIGGER);
    WriteReg(Register::WAV_SEQ_1, effect);
    WriteReg(Register::WAV_SEQ_2, 0);
    WriteReg(Register::GO, 0x01);
}

void Driver::PlaySequence(const uint8_t* effects, uint8_t count)
{
    WriteReg(Register::MODE, Mode::INTERNAL_TRIGGER);

    uint8_t n = count > 8 ? 8 : count;
    for(uint8_t i = 0; i < n; i++)
        WriteReg(Register::WAV_SEQ_1 + i, effects[i]);
    if(n < 8)
        WriteReg(Register::WAV_SEQ_1 + n, 0);

    WriteReg(Register::GO, 0x01);
}

void Driver::Stop()
{
    WriteReg(Register::GO, 0x00);
}

void Driver::SetRealtime(int8_t value)
{
    WriteReg(Register::MODE, Mode::REALTIME);
    WriteReg(Register::RT_PLAYBACK, (uint8_t)value);
}

void Driver::SetStandby(bool standby)
{
    if(standby)
        WriteReg(Register::MODE, Mode::STANDBY_BIT);
    else
        WriteReg(Register::MODE, Mode::INTERNAL_TRIGGER);
}

bool Driver::AutoCalibrate(uint8_t rated_voltage, uint8_t od_clamp)
{
    WriteReg(Register::RATED_VOLTAGE, rated_voltage);
    WriteReg(Register::OD_CLAMP_V, od_clamp);
    WriteReg(Register::MODE, Mode::AUTO_CALIBRATION);
    WriteReg(Register::GO, 0x01);

    for(int i = 0; i < 100; i++)
    {
        System::Delay(50);
        if((ReadReg(Register::GO) & 0x01) == 0)
            return (ReadReg(Register::STATUS) & 0x08) == 0;
    }
    return false;
}

bool Driver::WriteReg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = {reg, val};
    return i2c_->TransmitBlocking(I2CAddr::DRV2605L, buf, 2, 100)
           == I2CHandle::Result::OK;
}

uint8_t Driver::ReadReg(uint8_t reg)
{
    uint8_t val = 0;
    i2c_->TransmitBlocking(I2CAddr::DRV2605L, &reg, 1, 100);
    i2c_->ReceiveBlocking(I2CAddr::DRV2605L, &val, 1, 100);
    return val;
}

}  // namespace Haptic
