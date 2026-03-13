#include "Test.h"
#include "esp_clk_tree.h"

void CheckCpuFreq()
{
    uint32_t freq_hz;
    // Get the frequency of the CPU clock
    esp_clk_tree_src_get_freq_hz(SOC_MOD_CLK_CPU, ESP_CLK_TREE_SRC_FREQ_PRECISION_EXACT, &freq_hz);
    printf("CPU Clock: %lu Hz\n", freq_hz);
}

extern "C" void app_main(void)
{
    printf("Build Date: %s\n", __DATE__);
    printf("Build Time: %s\n", __TIME__);
    printf("hallo van esp\n");
    // CheckCpuFreq();

    MyLed led;


    while (true)
    {
        led.on();

        led.off();
    }
}