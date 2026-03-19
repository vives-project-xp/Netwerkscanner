#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

// Hardware Definitions
#define LCD_HOST    SPI2_HOST
#define PIN_NUM_MOSI GPIO_NUM_24
#define PIN_NUM_CLK  GPIO_NUM_23
#define PIN_NUM_CS   GPIO_NUM_27
#define PIN_NUM_DC   GPIO_NUM_8
#define PIN_NUM_RST  GPIO_NUM_26
#define PIN_NUM_BL   GPIO_NUM_15

#define LCD_WIDTH  240
#define LCD_HEIGHT 240

spi_device_handle_t spi;


// Sends a command to the ST7789
static void lcd_cmd(const uint8_t cmd) {
    gpio_set_level(PIN_NUM_DC, 0);
    spi_transaction_t t = {};
    t.length = 8;
    t.tx_buffer = &cmd;
    spi_device_polling_transmit(spi, &t);
}

// Sends data to the ST7789
static void lcd_data(const uint8_t *data, int len) {
    if (len == 0) return;
    gpio_set_level(PIN_NUM_DC, 1);
    spi_transaction_t t = {};
    t.length = (size_t)len * 8;
    t.tx_buffer = data;
    spi_device_polling_transmit(spi, &t);
}
static void set_addr_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    uint8_t data[4];

    lcd_cmd(0x2A);
    data[0] = x0 >> 8;
    data[1] = x0 & 0xFF;
    data[2] = x1 >> 8;
    data[3] = x1 & 0xFF;
    lcd_data(data, 4);

    lcd_cmd(0x2B);
    data[0] = y0 >> 8;
    data[1] = y0 & 0xFF;
    data[2] = y1 >> 8;
    data[3] = y1 & 0xFF;
    lcd_data(data, 4);

    lcd_cmd(0x2C);
}
// Clears the entire screen to a single color
static void fill_screen(uint16_t color) {
    // 1. Prepare a buffer for one horizontal line to save RAM
    uint16_t *line_buffer = (uint16_t *)heap_caps_malloc(LCD_WIDTH * sizeof(uint16_t), MALLOC_CAP_DMA);
    uint16_t color_swapped = (color << 8) | (color >> 8); // ST7789 is Big-Endian

    for (int i = 0; i < LCD_WIDTH; i++) {
        line_buffer[i] = color_swapped;
    }

    set_addr_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);
    gpio_set_level(PIN_NUM_DC, 1);

    // 2. Send the screen data line-by-line instead of pixel-by-pixel
    for (int y = 0; y < LCD_HEIGHT; y++) {
        spi_transaction_t t = {
            .length = LCD_WIDTH * 16,     // 16 bits per pixel
            .tx_buffer = line_buffer,
        };
        spi_device_transmit(spi, &t);
    }

    free(line_buffer);
}

// Sets the "active" drawing area to a single point (x, y)
void draw_pixel(uint16_t x, uint16_t y, uint16_t color) {
    uint8_t data[4];
    
    // Column Address Set (0x2A)
    lcd_cmd(0x2A);
    data[0] = x >> 8; data[1] = x & 0xFF;
    data[2] = x >> 8; data[3] = x & 0xFF;
    lcd_data(data, 4);

    // Row Address Set (0x2B)
    lcd_cmd(0x2B);
    data[0] = y >> 8; data[1] = y & 0xFF;
    data[2] = y >> 8; data[3] = y & 0xFF;
    lcd_data(data, 4);

    // Write to RAM (0x2C)
    lcd_cmd(0x2C);
    uint8_t color_data[2] = { (uint8_t)(color >> 8), (uint8_t)(color & 0xFF) };
    lcd_data(color_data, 2);
}
void DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy; // Error accumulation

    while (1) {
        draw_pixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void st7789_init() {
    // Reset display
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));

    lcd_cmd(0x11); // Sleep out
    vTaskDelay(pdMS_TO_TICKS(120));
    lcd_cmd(0x3A); // Interface pixel format (16-bit)
    uint8_t colmod = 0x55;
    lcd_data(&colmod, 1);
    lcd_cmd(0x29); // Display ON
}

void ScreenTest(void) {
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
    gpio_set_direction(PIN_NUM_DC,  GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_BL,  GPIO_MODE_OUTPUT);

    spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(LCD_HOST, &devcfg, &spi);

    gpio_set_level(PIN_NUM_BL, 1); // Turn on backlight
    st7789_init();

    fill_screen(0xf700);
    // Draw one white pixel in the center
    draw_pixel(120, 120, 0xFFFF);
    
    for (int i = 0; i < 240; i+=10)
    {
        DrawLine(i,10,120,120,0xFFFF);
    }
    



}