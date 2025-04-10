#include "FreeRTOSConfig.h"
#include "boards/pico_w.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"

#include "shared.h"


static inline void setCS(uint cs, int set) {
    asm volatile("nop \n nop \n nop"); // FIXME
    gpio_put(cs, set);
    asm volatile("nop \n nop \n nop"); // FIXME
}

void read(Action_t action) {
    uint8_t buff[action.len];
    uint8_t readAddr[4] = { 0x03, uint8_t(action.addr >> 16), uint8_t(action.addr >> 8), uint8_t(action.addr) };

    setCS(PICO_DEFAULT_SPI_CSN_PIN, 0);

    spi_write_blocking(spi_default, readAddr, 4);
    spi_read_blocking(spi_default, 0, buff, action.len);

    setCS(PICO_DEFAULT_SPI_CSN_PIN, 1);

    for(uint8_t value : buff) {
        xQueueSend(RActionQueue, &value, MAX_DELAY);
    }
}

void write(Action_t action) {
    uint8_t writeAddr[4] = { 0x03, uint8_t(action.addr >> 16), uint8_t(action.addr >> 8), uint8_t(action.addr) };

    setCS(PICO_DEFAULT_SPI_CSN_PIN, 0);

    spi_write_blocking(spi_default, writeAddr, 4);
    spi_write_blocking(spi_default, action.data, action.len);

    setCS(PICO_DEFAULT_SPI_CSN_PIN, 1);
}

void vProgrammerTask(void*) {
    spi_init(spi_default, 1000 * 1000);
    gpio_set_function(PICO_DEFAULT_SPI_RX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(PICO_DEFAULT_SPI_TX_PIN, GPIO_FUNC_SPI);

    gpio_init(PICO_DEFAULT_SPI_CSN_PIN);
    gpio_put(PICO_DEFAULT_SPI_CSN_PIN, 1);
    gpio_set_dir(PICO_DEFAULT_SPI_CSN_PIN, GPIO_OUT);


    while(true) {
        Action_t action;
        xQueueReceive(QActionQueue, &action, MAX_DELAY);

        switch(action.proc) {

        case Action_t::Read:
            read(action);
            break;
        case Action_t::Write:
            write(action);
            break;
        case Action_t::Pin: {
            uint8_t status = action.data[0];
            setCS(PICO_DEFAULT_SPI_CSN_PIN, status);
        } break;
        }
    }
}
