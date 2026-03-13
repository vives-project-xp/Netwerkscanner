#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_system.h"

#define PIN_NUM_MOSI GPIO_NUM_24
#define PIN_NUM_CLK GPIO_NUM_23
#define PIN_NUM_CS GPIO_NUM_27
#define PIN_NUM_DC GPIO_NUM_8
#define PIN_NUM_RST GPIO_NUM_26

#define LCD_HOST SPI2_HOST


spi_device_handle_t spi;

void draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    uint8_t data[2] = {
        color >> 8,
        color & 0xFF
    };

    set_addr_window(x, y, x, y);
    lcd_data(data, 2);
}

static void lcd_cmd(const uint8_t cmd)
{
    gpio_set_level(PIN_NUM_DC, 0);

    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd};

    spi_device_transmit(spi, &t);
}

static void lcd_data(const uint8_t *data, int len)
{
    if (len == 0)
        return;

    gpio_set_level(PIN_NUM_DC, 1);

    spi_transaction_t t = {
        .length = (size_t)len * 8,
        .tx_buffer = data};

    spi_device_transmit(spi, &t);
}

static void lcd_reset()
{
    gpio_set_level(PIN_NUM_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(PIN_NUM_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
}

static void st7789_init()
{
    lcd_reset();

    lcd_cmd(0x11); // sleep out
    vTaskDelay(pdMS_TO_TICKS(120));

    uint8_t data;

    lcd_cmd(0x36); // memory access
    data = 0x00;
    lcd_data(&data, 1);

    lcd_cmd(0x3A); // color mode
    data = 0x55;   // 16-bit color
    lcd_data(&data, 1);

    lcd_cmd(0x21); // display inversion on

    lcd_cmd(0x13); // normal display on

    lcd_cmd(0x29); // display on
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

static void fill_screen(uint16_t color)
{
    uint8_t data[2] = {
        (uint8_t)(color >> 8),
        (uint8_t)(color & 0xFF)};

    set_addr_window(0, 0, 239, 239);

    gpio_set_level(PIN_NUM_DC, 1);

    for (int i = 0; i < 240 * 240; i++)
    {
        spi_transaction_t t = {
            .length = 16,
            .tx_buffer = data};
        spi_device_transmit(spi, &t);
    }
}

void ScreenTest(void)
{
    spi_bus_config_t buscfg = {};

    buscfg.miso_io_num = -1;
    buscfg.mosi_io_num = PIN_NUM_MOSI;
    buscfg.sclk_io_num = PIN_NUM_CLK;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;

    spi_device_interface_config_t devcfg = {};

    devcfg.clock_speed_hz = 40000000;
    devcfg.mode = 0;
    devcfg.spics_io_num = PIN_NUM_CS;
    devcfg.queue_size = 7;

    gpio_set_direction(PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(PIN_NUM_RST, GPIO_MODE_OUTPUT);

    spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO);
    spi_bus_add_device(LCD_HOST, &devcfg, &spi);

#define PIN_NUM_BL GPIO_NUM_15

    gpio_set_direction(PIN_NUM_BL, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_NUM_BL, 1);

    st7789_init();

    while (1)
    {
        fill_screen(0xF800); // red
        vTaskDelay(pdMS_TO_TICKS(1000));

        fill_screen(0x07E0); // green
        vTaskDelay(pdMS_TO_TICKS(1000));

        fill_screen(0x001F); // blue
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}