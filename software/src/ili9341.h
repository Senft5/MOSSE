#pragma once
#include "daisy_seed.h"
#include "pin_defs.h"

namespace Display {

constexpr uint16_t WIDTH  = 240;
constexpr uint16_t HEIGHT = 320;

constexpr uint16_t BLACK   = 0x0000;
constexpr uint16_t WHITE   = 0xFFFF;
constexpr uint16_t RED     = 0xF800;
constexpr uint16_t GREEN   = 0x07E0;
constexpr uint16_t BLUE    = 0x001F;
constexpr uint16_t YELLOW  = 0xFFE0;
constexpr uint16_t CYAN    = 0x07FF;
constexpr uint16_t MAGENTA = 0xF81F;
constexpr uint16_t ORANGE  = 0xFD20;
constexpr uint16_t GRAY    = 0x8410;

inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

class Driver {
public:
    bool Init(daisy::DaisySeed& seed);
    void FillScreen(uint16_t color);
    void FillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                  uint16_t color);
    void DrawPixel(uint16_t x, uint16_t y, uint16_t color);
    void DrawHLine(uint16_t x, uint16_t y, uint16_t w, uint16_t color);
    void DrawVLine(uint16_t x, uint16_t y, uint16_t h, uint16_t color);
    void DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                  uint16_t color);
    void DrawChar(uint16_t x, uint16_t y, char c, uint16_t fg, uint16_t bg);
    void DrawString(uint16_t x, uint16_t y, const char* str,
                    uint16_t fg, uint16_t bg);
    void SetBacklight(uint8_t brightness);
    void SetRotation(uint8_t rotation);

private:
    daisy::SpiHandle spi_;
    daisy::GPIO dc_pin_;
    daisy::GPIO rst_pin_;
    daisy::GPIO cs_pin_;
    daisy::GPIO bl_pin_;
    uint8_t rotation_;

    void WriteCommand(uint8_t cmd);
    void WriteData(uint8_t data);
    void WriteData16(uint16_t data);
    void SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

}  // namespace Display
