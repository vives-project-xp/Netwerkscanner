#ifndef SCREEN
#define SCREEN
void ScreenTest(void);
void GpioScreenSetup(void);
void St7789Init();
void FillScreen(uint16_t color);
void BacklightOn();
void DrawWifiIcon(uint16_t x, uint16_t y, uint16_t color);
void DrawStringFast(uint16_t x, uint16_t y, const char *str, uint16_t textColor, uint16_t bgColor, uint8_t size);
void DrawRectFilled(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t borderColor, uint16_t fillColor, uint8_t borderThickness);
#endif