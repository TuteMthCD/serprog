#include "FreeRTOSConfig.h"
#include "shared.h"

#include "pico/cyw43_arch.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"


TaskHandle_t USBHandle;
QueueHandle_t ActionQueue;

void vLedTask(void*) {

    if(cyw43_arch_init()) {
        printf("cyw43 error on init");
        vTaskDelete(NULL);
        return;
    } else {
        printf("cyw43 init correctly");
    }


    bool pin = 0;
    while(true) {
        pin = !pin;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, pin);
        vTaskDelay(1000);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(1500);

    /* INIT */

    ActionQueue = xQueueCreate(ACTION_QUEUE_LEN, sizeof(Action_t*));

    xTaskCreate(&vLedTask, "vLedTask", 256, NULL, TASK_IDLE, NULL);
    xTaskCreate(&vUSBTask, "vUSBTask", 2 * 1024, NULL, TASK_MEDIUM, &USBHandle);
    xTaskCreate(&vProgrammerTask, "read", 256, NULL, TASK_HIGH, NULL);

    vTaskStartScheduler();
}
