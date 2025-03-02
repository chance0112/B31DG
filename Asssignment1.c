#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define OUTPUT_ENABLE GPIO_NUM_4
#define OUTPUT_SELECT GPIO_NUM_5
#define DATA_PIN GPIO_NUM_18
#define SYNC_PIN GPIO_NUM_19

#define PRODUCTION 1
#if PRODUCTION
  #define TIME_SCALE 1
#else
  #define TIME_SCALE 1000
#endif

void generate_data_signal(bool alternativeMode) {
    int a = 1200 * TIME_SCALE;
    int b = 900 * TIME_SCALE;
    int c = alternativeMode ? 13 : 10;
    int d = 3000 * TIME_SCALE;
    int syncOn = 50 * TIME_SCALE;

    for (int i = 0; i < c; i++) {
        int ton = a + (i * 50 * TIME_SCALE);
        gpio_set_level(DATA_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(ton / 1000));
        gpio_set_level(DATA_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(b / 1000));
    }

    vTaskDelay(pdMS_TO_TICKS(d / 1000));

    gpio_set_level(SYNC_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(syncOn / 1000));
    gpio_set_level(SYNC_PIN, 0);
}

void app_main() {
    gpio_pad_select_gpio(OUTPUT_ENABLE);
    gpio_set_direction(OUTPUT_ENABLE, GPIO_MODE_INPUT);
    gpio_pad_select_gpio(OUTPUT_SELECT);
    gpio_set_direction(OUTPUT_SELECT, GPIO_MODE_INPUT);
    gpio_pad_select_gpio(DATA_PIN);
    gpio_set_direction(DATA_PIN, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SYNC_PIN);
    gpio_set_direction(SYNC_PIN, GPIO_MODE_OUTPUT);

    bool dataEnabled = false;
    bool alternativeMode = false;
    
    while (1) {
        if (gpio_get_level(OUTPUT_ENABLE)) {
            dataEnabled = !dataEnabled;
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (gpio_get_level(OUTPUT_SELECT)) {
            alternativeMode = !alternativeMode;
            vTaskDelay(pdMS_TO_TICKS(200));
        }

        if (dataEnabled) {
            generate_data_signal(alternativeMode);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
