#include <stdio.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"

typedef struct {
    int pin;
    bool level;
    esp_timer_handle_t timer;
} led_t;

static void led_timer_callback(void* arg);

void led_init(led_t* led, int pin)
{
    led->pin = pin;
    led->level = 0;

    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);

    const esp_timer_create_args_t timer_args = {
        .callback = led_timer_callback,
        .arg = led,
        .name = "led_timer"
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &led->timer));
}

void led_start(led_t* led, int period_us)
{
    ESP_ERROR_CHECK(esp_timer_start_periodic(led->timer, period_us));
}

void led_stop(led_t* led)
{
    esp_timer_stop(led->timer); // boleh gagal → ignore
    gpio_set_level(led->pin, 0);
}

void app_main(void)
{
    static led_t led1, led2, led3;

    led_init(&led1, 23);
    led_init(&led2, 19);
    led_init(&led3, 18);

    led_start(&led1, 500000);  // 0.5 detik
    led_start(&led2, 200000);  // lebih cepat
    led_start(&led3, 1000000); // lebih lambat
}

static void led_timer_callback(void* arg)
{
    led_t* led = (led_t*) arg;

    led->level = !led->level;
    gpio_set_level(led->pin, led->level);
}
