#ifndef SCREEN
#define SCREEN
void ScreenTest(void);
void GpioScreenSetup(void);
void St7789Init();
void FillScreen(uint16_t color);
void BacklightOn();
#endif