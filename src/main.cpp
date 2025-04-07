#include "shared.h"

#include "pico/cyw43_arch.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"

void vLedTask(void*) {
    bool pin = 0;
    while(true) {
        pin = !pin;
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, pin);
        vTaskDelay(1000);
    }
}

int main() {
    stdio_init_all();

    /*      INIT LED    */

    if(cyw43_arch_init()) {
        printf("cyw43 error on init");
        return -1;
    } else {
        printf("cyw43 init correctly");
    }

    xTaskCreate(&vLedTask, "vLedTask", 256, NULL, TASK_IDLE, NULL);

    /*      FINSH LED   */

    QActionQueue = xQueueCreate(QACTION_QUEUE_LEN, sizeof(Action_t));
    RActionQueue = xQueueCreate(RACTION_QUEUE_LEN, sizeof(char));

    xTaskCreate(&vUSBTask, "vUSBTask", 2 * 1024, NULL, TASK_MEDIUM, NULL);


    vTaskStartScheduler();
}
