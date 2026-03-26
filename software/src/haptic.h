#pragma once
#include "daisy_seed.h"
#include "pin_defs.h"

namespace Haptic {

enum Register : uint8_t {
    STATUS        = 0x00,
    MODE          = 0x01,
    RT_PLAYBACK   = 0x02,
    LIBRARY_SEL   = 0x03,
    WAV_SEQ_1     = 0x04,
    WAV_SEQ_2     = 0x05,
    WAV_SEQ_3     = 0x06,
    WAV_SEQ_4     = 0x07,
    WAV_SEQ_5     = 0x08,
    WAV_SEQ_6     = 0x09,
    WAV_SEQ_7     = 0x0A,
    WAV_SEQ_8     = 0x0B,
    GO            = 0x0C,
    OD_CLAMP      = 0x0D,
    SUSTAIN_P     = 0x0E,
    SUSTAIN_N     = 0x0F,
    BRAKE         = 0x10,
    A2V_CTRL      = 0x11,
    A2V_MIN_IN    = 0x12,
    A2V_MAX_IN    = 0x13,
    A2V_MIN_OUT   = 0x14,
    A2V_MAX_OUT   = 0x15,
    RATED_VOLTAGE = 0x16,
    OD_CLAMP_V    = 0x17,
    A_CAL_COMP    = 0x18,
    A_CAL_BEMF    = 0x19,
    FEEDBACK_CTRL = 0x1A,
    CONTROL1      = 0x1B,
    CONTROL2      = 0x1C,
    CONTROL3      = 0x1D,
    CONTROL4      = 0x1E,
    CONTROL5      = 0x1F,
    OL_LRA_PERIOD = 0x20,
    VBAT          = 0x21,
    LRA_RESONANCE = 0x22,
};

enum Mode : uint8_t {
    INTERNAL_TRIGGER  = 0x00,
    EXT_EDGE_TRIGGER  = 0x01,
    EXT_LEVEL_TRIGGER = 0x02,
    PWM_ANALOG        = 0x03,
    AUDIO_TO_VIBE     = 0x04,
    REALTIME          = 0x05,
    DIAGNOSTICS       = 0x06,
    AUTO_CALIBRATION  = 0x07,
    STANDBY_BIT       = 0x40,
};

enum Library : uint8_t {
    LIB_EMPTY    = 0x00,
    LIB_TS2200_A = 0x01,
    LIB_TS2200_B = 0x02,
    LIB_TS2200_C = 0x03,
    LIB_TS2200_D = 0x04,
    LIB_TS2200_E = 0x05,
    LIB_LRA      = 0x06,
};

enum Effect : uint8_t {
    STRONG_CLICK     = 1,
    STRONG_CLICK_60  = 2,
    STRONG_CLICK_30  = 3,
    SHARP_CLICK      = 4,
    SHARP_CLICK_60   = 5,
    SOFT_BUMP        = 7,
    DOUBLE_CLICK     = 10,
    TRIPLE_CLICK     = 12,
    SOFT_FUZZ        = 13,
    BUZZ_1           = 47,
    BUZZ_2           = 48,
    PULSING_STRONG   = 52,
    PULSING_SHARP    = 58,
    TRANSITION_CLICK = 64,
    SHORT_BUZZ       = 71,
    SMOOTH_HUM       = 94,
    ALERT_750MS      = 116,
    ALERT_1000MS     = 117,
};

class Driver {
public:
    bool Init(daisy::I2CHandle& i2c, bool use_lra = false);
    void PlayEffect(uint8_t effect);
    void PlaySequence(const uint8_t* effects, uint8_t count);
    void Stop();
    void SetRealtime(int8_t value);
    void SetStandby(bool standby);
    bool AutoCalibrate(uint8_t rated_voltage, uint8_t od_clamp);

private:
    daisy::I2CHandle* i2c_;
    bool use_lra_;

    bool WriteReg(uint8_t reg, uint8_t val);
    uint8_t ReadReg(uint8_t reg);
};

}  // namespace Haptic
