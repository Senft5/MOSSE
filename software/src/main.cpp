#include "daisy_seed.h"
#include "daisysp.h"
#include "pin_defs.h"
#include "ili9341.h"
#include "touch.h"
#include "haptic.h"
#include <cstdio>

using namespace daisy;
using namespace daisysp;

static DaisySeed           seed;
static I2CHandle           i2c;
static Display::Driver     display;
static Touch::Controller   touch;
static Haptic::Driver      haptic;

static Oscillator osc;
static float      audio_freq = 440.f;
static float      audio_amp  = 0.f;
static bool       audio_on   = false;

static GPIO btn_n, btn_s, btn_e, btn_w;
static GPIO gpio_on;

static Touch::TouchState touch_state = {};
static Touch::TouchState prev_touch  = {};
static uint32_t          last_disp   = 0;

// button edges - kinda lazy to leave these file static but works
static bool       pbN, pbS, pbE, pbW;
static uint8_t    wf_idx = 0;

static void AudioCallback(AudioHandle::InputBuffer in,
                          AudioHandle::OutputBuffer out,
                          size_t                    size)
{
    (void)in;
    osc.SetFreq(audio_freq);
    osc.SetAmp(audio_amp);
    for(size_t i = 0; i < size; i++)
    {
        float s = osc.Process();
        out[0][i] = s;
        out[1][i] = s;
    }
}

struct BtnState
{
    bool n, s, e, w;
};

static BtnState ReadBtns()
{
    BtnState b;
    b.n = !btn_n.Read();
    b.s = !btn_s.Read();
    b.e = !btn_e.Read();
    b.w = !btn_w.Read();
    return b;
}

static void UpdateDisplay(const Touch::TouchState& ts, const BtnState& b)
{
    char tmp[32];

    display.DrawString(10, 10, "TOUCH:", Display::WHITE, Display::BLACK);
    for(int i = 0; i < 8; i++)
    {
        bool on = (ts.keys >> i) & 1;
        display.FillRect(10 + i * 28, 30, 24, 24,
                         on ? Display::GREEN : Display::GRAY);
    }

    display.DrawString(10, 70, "BTNS:", Display::WHITE, Display::BLACK);
    display.FillRect(100, 90, 24, 24, b.n ? Display::CYAN : Display::GRAY);
    display.FillRect(100, 140, 24, 24, b.s ? Display::CYAN : Display::GRAY);
    display.FillRect(130, 115, 24, 24, b.e ? Display::CYAN : Display::GRAY);
    display.FillRect(70, 115, 24, 24, b.w ? Display::CYAN : Display::GRAY);

    display.DrawString(10, 180, "FREQ:", Display::WHITE, Display::BLACK);
    snprintf(tmp, sizeof(tmp), "%d Hz  ", (int)audio_freq);
    display.DrawString(80, 180, tmp, Display::YELLOW, Display::BLACK);

    display.DrawString(10, 200, "AUDIO:", Display::WHITE, Display::BLACK);
    display.DrawString(90, 200, audio_on ? "ON " : "OFF",
                       audio_on ? Display::GREEN : Display::RED,
                       Display::BLACK);
}

static bool InitPeripherals()
{
    I2CHandle::Config i2c_cfg;
    i2c_cfg.periph          = I2CHandle::Config::Peripheral::I2C_1;
    i2c_cfg.speed           = I2CHandle::Config::Speed::I2C_400KHZ;
    i2c_cfg.pin_config.scl  = seed.GetPin(Pins::I2C_SCL);
    i2c_cfg.pin_config.sda  = seed.GetPin(Pins::I2C_SDA);
    i2c_cfg.mode            = I2CHandle::Config::Mode::I2C_MASTER;
    if(i2c.Init(i2c_cfg) != I2CHandle::Result::OK)
        return false;

    gpio_on.Init(seed.GetPin(Pins::GPIO_ON), GPIO::Mode::OUTPUT);
    gpio_on.Write(true); // hold pwr

    btn_n.Init(seed.GetPin(Pins::BTN_N), GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    btn_s.Init(seed.GetPin(Pins::BTN_S), GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    btn_e.Init(seed.GetPin(Pins::BTN_E), GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
    btn_w.Init(seed.GetPin(Pins::BTN_W), GPIO::Mode::INPUT, GPIO::Pull::PULLUP);

    return true;
}

int main(void)
{
    seed.Init();
    seed.SetAudioBlockSize(48);
    seed.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    if(!InitPeripherals())
    {
        for(;;)
        {
            seed.SetLed(true);
            System::Delay(100);
            seed.SetLed(false);
            System::Delay(100);
        }
    }

    bool ok_disp = display.Init(seed);
    if(ok_disp)
    {
        display.FillScreen(Display::BLACK);
        display.DrawString(10, 150, "Initializing...", Display::WHITE,
                           Display::BLACK);
    }

    bool ok_touch = touch.Init(i2c, seed);
    if(ok_disp)
    {
        display.DrawString(10, 170, ok_touch ? "Touch: OK" : "Touch: FAIL",
                           ok_touch ? Display::GREEN : Display::RED,
                           Display::BLACK);
    }

    bool ok_hap = haptic.Init(i2c, false); // erm
    if(ok_disp)
    {
        display.DrawString(10, 190, ok_hap ? "Haptic: OK" : "Haptic: FAIL",
                           ok_hap ? Display::GREEN : Display::RED,
                           Display::BLACK);
    }

    System::Delay(1500);

    osc.Init(seed.AudioSampleRate());
    osc.SetWaveform(Oscillator::WAVE_SIN);
    osc.SetFreq(440.f);
    osc.SetAmp(0.f);
    seed.StartAudio(AudioCallback);

    if(ok_disp)
    {
        display.FillScreen(Display::BLACK);
        display.DrawString(10, 240, "Touch a pad to play", Display::GRAY,
                           Display::BLACK);
        display.DrawString(10, 260, "N/S = freq  E = on/off", Display::GRAY,
                           Display::BLACK);
    }

    if(ok_hap)
        haptic.PlayEffect(Haptic::STRONG_CLICK);

    for(;;)
    {
        uint32_t now = System::GetNow();

        if(ok_touch)
        {
            prev_touch  = touch_state;
            touch_state = touch.Read();

            uint16_t rising = touch_state.keys & ~prev_touch.keys;
            if(rising && ok_hap)
                haptic.PlayEffect(Haptic::SHARP_CLICK);

            static const float notes[] = {
                261.63f, 293.66f, 329.63f, 349.23f,
                392.f,   440.f,   493.88f, 523.25f,
            };

            if(touch_state.any_touch)
            {
                for(int k = 0; k < 8; k++)
                {
                    if(touch_state.keys & (1 << k))
                    {
                        audio_freq = notes[k];
                        audio_amp  = 0.5f;
                        audio_on   = true;
                        break;
                    }
                }
            }
            else
            {
                audio_amp = 0.f;
                audio_on  = false;
            }
        }

        BtnState bt = ReadBtns();

        if(bt.n && !pbN)
        {
            audio_freq *= 1.05946f;
            if(audio_freq > 4000.f)
                audio_freq = 4000.f;
            if(ok_hap)
                haptic.PlayEffect(Haptic::SHARP_CLICK);
        }
        if(bt.s && !pbS)
        {
            audio_freq /= 1.05946f;
            if(audio_freq < 20.f)
                audio_freq = 20.f;
            if(ok_hap)
                haptic.PlayEffect(Haptic::SHARP_CLICK);
        }
        if(bt.e && !pbE)
        {
            audio_on  = !audio_on;
            audio_amp = audio_on ? 0.5f : 0.f;
            if(ok_hap)
                haptic.PlayEffect(Haptic::DOUBLE_CLICK);
        }
        if(bt.w && !pbW)
        {
            wf_idx = (uint8_t)((wf_idx + 1) % 4);
            switch(wf_idx)
            {
            case 0:
                osc.SetWaveform(Oscillator::WAVE_SIN);
                break;
            case 1:
                osc.SetWaveform(Oscillator::WAVE_SAW);
                break;
            case 2:
                osc.SetWaveform(Oscillator::WAVE_SQUARE);
                break;
            case 3:
                osc.SetWaveform(Oscillator::WAVE_TRI);
                break;
            }
            if(ok_hap)
                haptic.PlayEffect(Haptic::SOFT_BUMP);
        }

        pbN = bt.n;
        pbS = bt.s;
        pbE = bt.e;
        pbW = bt.w;

        if(ok_disp && (now - last_disp > 50))
        {
            last_disp = now;
            UpdateDisplay(touch_state, bt);
        }

        GPIO pwr_sw;
        pwr_sw.Init(seed.GetPin(Pins::GPIO_OFF), GPIO::Mode::INPUT,
                    GPIO::Pull::PULLUP);
        if(!pwr_sw.Read())
        {
            if(ok_hap)
            {
                haptic.PlayEffect(Haptic::ALERT_750MS);
                System::Delay(800);
            }
            if(ok_disp)
            {
                display.SetBacklight(0);
                display.FillScreen(Display::BLACK);
            }
            gpio_on.Write(false);
            for(;;)
            {
                seed.SetLed(false);
                System::Delay(1000);
            }
        }

        System::Delay(5);
    }
}
