#include "Test.h"
#include "esp_clk_tree.h"

void check_speed() {
    uint32_t freq_hz;
    // Get the frequency of the CPU clock
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &freq_hz);
    printf("CPU Clock: %lu Hz\n", freq_hz);
}

extern "C" void app_main(void)
{
    printf("hallo van esp\n");
    check_speed();

    MyLed led;

    while (true)
    {
        led.on();

        led.off();
    }
}