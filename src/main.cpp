#include "FreeRTOS.h"
#include "pico/cyw43_arch.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "task.h"
#include <stdio.h>

void led_task(void *) {
  bool pin = 0;
  while (true) {
    pin = !pin;
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, pin);
    vTaskDelay(1000);
  }
}

int main() {
  stdio_init_all();

  if (cyw43_arch_init()) {
    printf("cyw43 error on init");
    return -1;
  } else {
    printf("cyw43 init correctly");
  }

  xTaskCreate(&led_task, "LED_Task", 256, NULL, 1, NULL);

  vTaskStartScheduler();
}
