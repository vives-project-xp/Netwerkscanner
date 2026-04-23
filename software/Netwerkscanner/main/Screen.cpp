#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include <math.h>
#include <string.h>

// Hardware Definitions
#define LCD_HOST SPI2_HOST
#define PIN_NUM_MOSI GPIO_NUM_24
#define PIN_NUM_CLK GPIO_NUM_23
#define PIN_NUM_CS GPIO_NUM_27
#define PIN_NUM_DC GPIO_NUM_8
#define PIN_NUM_RST GPIO_NUM_26
#define PIN_NUM_BL GPIO_NUM_15

#define LCD_WIDTH 240
#define LCD_HEIGHT 240

spi_device_handle_t spi;

extern const uint8_t font5x7[][5];





// Sends a command to the ST7789
void LcdCommand(const uint8_t cmd)
{
    gpio_set_level(PIN_NUM_DC, 0);
    spi_transaction_t t = {};
    t.length = 8;
    t.tx_buffer = &cmd;
    spi_device_polling_transmit(spi, &t);
}

// Sends data to the ST7789
void LcdData(const uint8_t *data, int len)
{
    if (len == 0)
        return;
    gpio_set_level(PIN_NUM_DC, 1);
    spi_transaction_t t = {};
    t.length = (size_t)len * 8;
    t.tx_buffer = data;
    spi_device_polling_transmit(spi, &t);
}
void SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];

    LcdCommand(0x2A);
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;
    LcdData(data, 4);

    LcdCommand(0x2B);
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;
    LcdData(data, 4);

    LcdCommand(0x2C);
}
// Clears the entire screen to a single color
void FillScreen(uint16_t color)
{
    // 1. Prepare a buffer for one horizontal line to save RAM
    uint16_t *line_buffer = (uint16_t *)heap_caps_malloc(LCD_WIDTH * sizeof(uint16_t), MALLOC_CAP_DMA);
    uint16_t color_swapped = (color << 8) | (color >> 8); // ST7789 is Big-Endian

    for (int i = 0; i < LCD_WIDTH; i++)
    {
        line_buffer[i] = color_swapped;
    }

    SetAddressWindow(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    gpio_set_level(PIN_NUM_DC, 1);

    // 2. Send the screen data line-by-line instead of pixel-by-pixel
    for (int y = 0; y < LCD_HEIGHT; y++)
    {
        spi_transaction_t t = {
            .length = LCD_WIDTH * 16, // 16 bits per pixel
            .tx_buffer = line_buffer,
        };
        spi_device_transmit(spi, &t);
    }

    free(line_buffer);
}

// Sets the "active" drawing area to a single point (x, y)
void DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t data[4];

    // Column Address Set (0x2A)
    LcdCommand(0x2A);
    data[0] = x >> 8;
    data[1] = x & 0xFF;
    data[2] = x >> 8;
    data[3] = x & 0xFF;
    LcdData(data, 4);

    // Row Address Set (0x2B)
    LcdCommand(0x2B);
    data[0] = y >> 8;
    data[1] = y & 0xFF;
    data[2] = y >> 8;
    data[3] = y & 0xFF;
    LcdData(data, 4);

    // Write to RAM (0x2C)
    LcdCommand(0x2C);
    uint8_t color_data[2] = {(uint8_t)(color >> 8), (uint8_t)(color & 0xFF)};
    LcdData(color_data, 2);
}
void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy; // Error accumulation

    while (1)
    {
        DrawPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2)
            break;

        int e2 = 2 * err;
        if (e2 >= dy)
        {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void DrawCharFastBG(uint16_t x, uint16_t y, char c, uint16_t textColor, uint16_t bgColor, uint8_t size) {
    if ((uint8_t)c >= 128) return;
    
    // Bereken de afmetingen (5 kolommen + 1 spatiekolom)
    uint16_t width = 6 * size;
    uint16_t height = 8 * size;
    
    // Maak een buffer voor de hele letter (2 bytes per pixel voor RGB565)
    uint16_t *char_buffer = (uint16_t *)heap_caps_malloc(width * height * 2, MALLOC_CAP_DMA);
    if (!char_buffer) return;

    // Kleuren omdraaien voor ST7789 (Big-Endian)
    uint16_t tColor = (textColor << 8) | (textColor >> 8);
    uint16_t bColor = (bgColor << 8) | (bgColor >> 8);

    // Vul de buffer kolom voor kolom
    for (int col = 0; col < 6; col++) {
        uint8_t line = (col < 5) ? font5x7[(uint8_t)c][col] : 0x00; // 6e kolom is altijd achtergrond
        
        for (int row = 0; row < 8; row++) {
            uint16_t color = (line & (1 << row)) ? tColor : bColor;
            
            // Schaal de pixel in de buffer
            for (int sy = 0; sy < size; sy++) {
                for (int sx = 0; sx < size; sx++) {
                    int buf_idx = ((row * size + sy) * width) + (col * size + sx);
                    char_buffer[buf_idx] = color;
                }
            }
        }
    }

    // Stuur de hele letter in één keer naar het scherm
    SetAddressWindow(x, y, x + width - 1, y + height - 1);
    gpio_set_level(PIN_NUM_DC, 1);
    
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = width * height * 16;
    t.tx_buffer = char_buffer;
    spi_device_transmit(spi, &t);

    free(char_buffer);
}
void DrawStringFast(uint16_t x, uint16_t y, const char *str, uint16_t textColor, uint16_t bgColor, uint8_t size) {
    while (*str) {
        DrawCharFastBG(x, y, *str, textColor, bgColor, size);
        x += 6 * size;
        str++;
    }
}
void DrawWifiIcon(uint16_t x, uint16_t y, uint16_t color) {
    // 1. De stip onderaan (het centrum)
    DrawPixel(x, y, color);
    DrawPixel(x-1, y-1, color);
    DrawPixel(x+1, y-1, color);
    DrawPixel(x, y-1, color);

    // 2. De drie bogen tekenen
    // r is de straal (radius) van elke boog
    for (int r = 6; r <= 18; r += 6) {
        // We lopen door de hoeken van 225 graden naar 315 graden (de bovenkant)
        for (float angle = 225; angle <= 315; angle += 1.5) {
            // Bereken de positie van de pixel (stille omzetting naar radialen)
            int px = x + (int)(r * cos(angle * M_PI / 180.0));
            int py = y + (int)(r * sin(angle * M_PI / 180.0));
            
            DrawPixel(px, py, color);
        }
    }
}
void DrawRectFilled(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t borderColor, uint16_t fillColor, uint8_t borderThickness) {
    // 1. Maak een buffer voor het hele vierkant (RGB565 = 2 bytes per pixel)
    size_t bufferSize = w * h * sizeof(uint16_t);
    uint16_t *rect_buffer = (uint16_t *)heap_caps_malloc(bufferSize, MALLOC_CAP_DMA);
    if (!rect_buffer) return;

    // 2. Kleuren omdraaien voor ST7789 (Big-Endian correctie)
    uint16_t bColor = (borderColor << 8) | (borderColor >> 8);
    uint16_t fColor = (fillColor << 8) | (fillColor >> 8);

    // 3. Vul de buffer pixel voor pixel
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            // Check of de huidige pixel op de rand ligt
            bool isBorder = (row < borderThickness) ||             // Bovenkant
                            (row >= h - borderThickness) ||      // Onderkant
                            (col < borderThickness) ||             // Linkerkant
                            (col >= w - borderThickness);        // Rechterkant

            rect_buffer[row * w + col] = isBorder ? bColor : fColor;
        }
    }

    // 4. Stuur de buffer naar het display
    SetAddressWindow(x, y, x + w - 1, y + h - 1);
    gpio_set_level(PIN_NUM_DC, 1); // Data mode
    
    spi_transaction_t t;
    memset(&t, 0, sizeof(t));
    t.length = w * h * 16;         // Lengte in bits
    t.tx_buffer = rect_buffer;
    spi_device_transmit(spi, &t);

    // 5. Ruim de buffer netjes op
    free(rect_buffer);
}
void St7789Init()
{
    // Reset display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    LcdCommand(0x11); // Sleep out
    vTaskDelay(pdMS_TO_TICKS(120));
    LcdCommand(0x3A); // Interface pixel format (16-bit)
    uint8_t colmod = 0x55;
    LcdData(&colmod, 1);

    LcdCommand(0x36);
    uint8_t madctl = 0x60; // 0x60 is Landschap (90 graden)
    LcdData(&madctl, 1);

    LcdCommand(0x21);// kleur juist zetten

    LcdCommand(0x29); // Display ON
}

void GpioScreenSetup()
{
    // SPI Bus Configuration
    spi_bus_config_t buscfg = {};
    buscfg.mosi_io_num = PIN_NUM_MOSI;
    buscfg.miso_io_num = -1;
    buscfg.sclk_io_num = PIN_NUM_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    // SPI Device Configuration
    spi_device_interface_config_t devcfg = {};
    devcfg.clock_speed_hz = 26 * 1000 * 1000;
    devcfg.mode = 0;
    devcfg.spics_io_num = PIN_NUM_CS;
    devcfg.queue_size = 7;

    // GPIO Setup
    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_BL, 0);

    spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(LCD_HOST, &devcfg, &spi);
}
void BacklightOn()
{
    gpio_set_level(PIN_NUM_BL, 1);
}

void ScreenTest(void)
{
    GpioScreenSetup();

    BacklightOn();
    St7789Init();

    FillScreen(0xf700);
    // Draw one white pixel in the center
    DrawPixel(120, 120, 0xFFFF);

    for (int i = 0; i < 240; i += 10)
    {
        DrawLine(i, 10, 120, 120, 0xFFFF);
    }
}

/* * RGB565 Kleurcodes (16-bit)
 * Formaat: RRRRRGGGGGGBBBBB
 * * Zwart:   0x0000
 * Wit:     0xFFFF
 * Rood:    0xF800
 * Groen:   0x07E0
 * Blauw:   0x001F
 * Geel:    0xFFE0 (Rood + Groen)
 * Cyaan:   0x07FF (Groen + Blauw)
 * Magenta: 0xF81F (Rood + Blauw)
 * Grijs:   0x8410
 * Oranje:  0xFD20
 */

const uint8_t font5x7[128][5] = {
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, // 0-3
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, // 4-7
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, // 8-11
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, // 12-15
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, // 16-19
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, // 20-23
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, // 24-27
    {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, {0,0,0,0,0}, // 28-31
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 32 Spatie
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // 33 !
    {0x00, 0x07, 0x00, 0x07, 0x00}, // 34 "
    {0x14, 0x7F, 0x14, 0x7F, 0x14}, // 35 #
    {0x24, 0x2A, 0x7F, 0x2A, 0x12}, // 36 $
    {0x23, 0x13, 0x08, 0x64, 0x62}, // 37 %
    {0x36, 0x49, 0x55, 0x22, 0x50}, // 38 &
    {0x00, 0x05, 0x03, 0x00, 0x00}, // 39 '
    {0x00, 0x1C, 0x22, 0x41, 0x00}, // 40 (
    {0x00, 0x41, 0x22, 0x1C, 0x00}, // 41 )
    {0x14, 0x08, 0x3E, 0x08, 0x14}, // 42 *
    {0x08, 0x08, 0x3E, 0x08, 0x08}, // 43 +
    {0x00, 0x50, 0x30, 0x00, 0x00}, // 44 ,
    {0x08, 0x08, 0x08, 0x08, 0x08}, // 45 -
    {0x00, 0x60, 0x60, 0x00, 0x00}, // 46 .
    {0x20, 0x10, 0x08, 0x04, 0x02}, // 47 /
    {0x3E, 0x51, 0x49, 0x45, 0x3E}, // 48 0
    {0x00, 0x42, 0x7F, 0x40, 0x00}, // 49 1
    {0x42, 0x61, 0x51, 0x49, 0x46}, // 50 2
    {0x21, 0x41, 0x45, 0x4B, 0x31}, // 51 3
    {0x18, 0x14, 0x12, 0x7F, 0x10}, // 52 4
    {0x27, 0x45, 0x45, 0x45, 0x39}, // 53 5
    {0x3C, 0x4A, 0x49, 0x49, 0x30}, // 54 6
    {0x01, 0x71, 0x09, 0x05, 0x03}, // 55 7
    {0x36, 0x49, 0x49, 0x49, 0x36}, // 56 8
    {0x06, 0x49, 0x49, 0x29, 0x1E}, // 57 9
    {0x00, 0x36, 0x36, 0x00, 0x00}, // 58 :
    {0x00, 0x56, 0x36, 0x00, 0x00}, // 59 ;
    {0x08, 0x14, 0x22, 0x41, 0x00}, // 60 <
    {0x14, 0x14, 0x14, 0x14, 0x14}, // 61 =
    {0x00, 0x41, 0x22, 0x14, 0x08}, // 62 >
    {0x02, 0x01, 0x51, 0x09, 0x06}, // 63 ?
    {0x32, 0x49, 0x79, 0x41, 0x3E}, // 64 @
    {0x7C, 0x12, 0x11, 0x12, 0x7C}, // 65 A
    {0x7F, 0x49, 0x49, 0x49, 0x36}, // 66 B
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 67 C
    {0x7F, 0x41, 0x41, 0x22, 0x1C}, // 68 D
    {0x7F, 0x49, 0x49, 0x49, 0x41}, // 69 E
    {0x7F, 0x09, 0x09, 0x09, 0x01}, // 70 F
    {0x3E, 0x41, 0x49, 0x49, 0x7A}, // 71 G
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 72 H
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // 73 I
    {0x20, 0x40, 0x41, 0x3F, 0x01}, // 74 J
    {0x7F, 0x08, 0x14, 0x22, 0x41}, // 75 K
    {0x7F, 0x40, 0x40, 0x40, 0x40}, // 76 L
    {0x7F, 0x02, 0x0C, 0x02, 0x7F}, // 77 M
    {0x7F, 0x04, 0x08, 0x10, 0x7F}, // 78 N
    {0x3E, 0x41, 0x41, 0x41, 0x3E}, // 79 O
    {0x7F, 0x09, 0x09, 0x09, 0x06}, // 80 P
    {0x3E, 0x41, 0x51, 0x21, 0x5E}, // 81 Q
    {0x7F, 0x09, 0x19, 0x29, 0x46}, // 82 R
    {0x46, 0x49, 0x49, 0x49, 0x31}, // 83 S
    {0x01, 0x01, 0x7F, 0x01, 0x01}, // 84 T
    {0x3F, 0x40, 0x40, 0x40, 0x3F}, // 85 U
    {0x1F, 0x20, 0x40, 0x20, 0x1F}, // 86 V
    {0x3F, 0x40, 0x38, 0x40, 0x3F}, // 87 W
    {0x63, 0x14, 0x08, 0x14, 0x63}, // 88 X
    {0x07, 0x08, 0x70, 0x08, 0x07}, // 89 Y
    {0x61, 0x51, 0x49, 0x45, 0x43}, // 90 Z
    {0x00, 0x7F, 0x41, 0x41, 0x00}, // 91 [
    {0x02, 0x04, 0x08, 0x10, 0x20}, // 92 "\"
    {0x00, 0x41, 0x41, 0x7F, 0x00}, // 93 ]
    {0x04, 0x02, 0x01, 0x02, 0x04}, // 94 ^
    {0x40, 0x40, 0x40, 0x40, 0x40}, // 95 _
    {0x00, 0x01, 0x02, 0x04, 0x00}, // 96 `
    {0x20, 0x54, 0x54, 0x54, 0x78}, // 97 a
    {0x7F, 0x48, 0x44, 0x44, 0x38}, // 98 b
    {0x38, 0x44, 0x44, 0x44, 0x20}, // 99 c
    {0x38, 0x44, 0x44, 0x48, 0x7F}, // 100 d
    {0x38, 0x54, 0x54, 0x54, 0x18}, // 101 e
    {0x08, 0x7E, 0x09, 0x01, 0x02}, // 102 f
    {0x0C, 0x52, 0x52, 0x52, 0x3E}, // 103 g
    {0x7F, 0x08, 0x04, 0x04, 0x78}, // 104 h
    {0x00, 0x44, 0x7D, 0x40, 0x00}, // 105 i
    {0x20, 0x40, 0x44, 0x3D, 0x00}, // 106 j
    {0x7F, 0x10, 0x28, 0x44, 0x00}, // 107 k
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // 108 l
    {0x7C, 0x04, 0x18, 0x04, 0x78}, // 109 m
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // 110 n
    {0x38, 0x44, 0x44, 0x44, 0x38}, // 111 o
    {0x7C, 0x14, 0x14, 0x14, 0x08}, // 112 p
    {0x08, 0x14, 0x14, 0x18, 0x7C}, // 113 q
    {0x7C, 0x08, 0x04, 0x04, 0x08}, // 114 r
    {0x48, 0x54, 0x54, 0x54, 0x20}, // 115 s
    {0x04, 0x3F, 0x44, 0x40, 0x20}, // 116 t
    {0x3C, 0x40, 0x40, 0x20, 0x7C}, // 117 u
    {0x1C, 0x20, 0x40, 0x20, 0x1C}, // 118 v
    {0x3C, 0x40, 0x30, 0x40, 0x3C}, // 119 w
    {0x44, 0x28, 0x10, 0x28, 0x44}, // 120 x
    {0x0C, 0x50, 0x50, 0x50, 0x3C}, // 121 y
    {0x44, 0x64, 0x54, 0x4C, 0x44}, // 122 z
    {0x00, 0x08, 0x36, 0x41, 0x00}, // 123 {
    {0x00, 0x00, 0x7F, 0x00, 0x00}, // 124 |
    {0x00, 0x41, 0x36, 0x08, 0x00}, // 125 }
    {0x02, 0x01, 0x02, 0x04, 0x02}, // 126 ~
    {0x00, 0x00, 0x00, 0x00, 0x00}  // 127 (DEL)
};