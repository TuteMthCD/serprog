#include "FreeRTOSConfig.h"
#include "shared.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include <array>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>


typedef enum {
    ACK = 0x06, // Acknowledge (respuesta positiva)
    NAK = 0x15, // Negative Acknowledge (respuesta negativa)

    CMD_NOP = 0x00,            // No hacer nada
    CMD_QUERY_IFACE = 0x01,    // Consultar versión de interfaz
    CMD_QUERY_COMMANDS = 0x02, // Consultar comandos soportados
    CMD_QUERY_NAME = 0x03,     // Consultar nombre del programador
    CMD_QUERY_SERBUF = 0x04,   // Consultar tamaño del buffer serie
    CMD_QUERY_BUSTYPE = 0x05,  // Consultar tipos de bus soportados
    CMD_QUERY_CHIPSIZE = 0x06, // Consultar tamaño del chip (formato 2^n)
    CMD_QUERY_OPBUF = 0x07,    // Consultar tamaño del buffer de operaciones
    CMD_QUERY_WRN_MAX = 0x08,  // Consultar longitud máxima para escritura múltiple

    CMD_READ_BYTE = 0x09,   // Leer un byte
    CMD_READ_NBYTES = 0x0A, // Leer N bytes

    CMD_OP_INIT = 0x0B,       // Inicializar buffer de operaciones
    CMD_OP_WRITE_BYTE = 0x0C, // Escribir byte con dirección en buffer
    CMD_OP_WRITE_N = 0x0D,    // Escribir N bytes en buffer
    CMD_OP_DELAY_US = 0x0E,   // Agregar retardo en microsegundos
    CMD_OP_EXECUTE = 0x0F,    // Ejecutar el buffer de operaciones

    CMD_SYNC_NOP = 0x10,      // NOP especial que responde NAK+ACK
    CMD_QUERY_RDN_MAX = 0x11, // Consultar cantidad máxima de lectura múltiple

    CMD_SET_BUSTYPE = 0x12,     // Establecer tipo(s) de bus a usar
    CMD_OP_SPI_TRANSFER = 0x13, // Realizar operación SPI
    CMD_SET_SPI_FREQ = 0x14,    // Configurar frecuencia del reloj SPI
    CMD_SET_PIN_STATE = 0x15    // Activar/desactivar drivers de salida
} CommandCode_t;

#define ADDR_LEN 3
#define N_LEN 3

constexpr size_t commandLength(const CommandCode_t& cmd) {
    switch(cmd) {
    case CMD_READ_BYTE:
        return ADDR_LEN;
    case CMD_READ_NBYTES:
        return ADDR_LEN + N_LEN;
    case CMD_OP_WRITE_BYTE:
        return ADDR_LEN + 1;
    case CMD_OP_WRITE_N:
        return ADDR_LEN + N_LEN; //+ data
    case CMD_OP_DELAY_US:
        return 4;
    case CMD_SET_BUSTYPE:
        return 1;
    case CMD_OP_SPI_TRANSFER:
        return 6; // + data
    case CMD_SET_SPI_FREQ:
        return 4;
    case CMD_SET_PIN_STATE:
        return 1;

    // other not have parameters
    default:
        break;
    }

    return 0;
}

// little endian
inline uint32_t littleEndian(uint8_t* p, size_t len) {
    uint32_t value = 0;
    for(int i = 0; i < len; i++) {
        value |= (uint32_t)p[i] << (8 * i);
    }
    return value;
}

void read(uint32_t addr, size_t len) {
    Action_t* p = new Action_t();

    p->handle = xTaskGetCurrentTaskHandle();

    p->proc = Action_t::Read;
    p->addr = addr;
    p->len = len;

    xQueueSend(ActionQueue, &p, MAX_DELAY);
    size_t notify = xTaskNotifyWait(0x01, 0x00, NULL, MAX_DELAY);

    if(notify) {
        printf("%c%.*s", ACK, p->len, p->data);
    } else {
        putchar(NAK);
    }

    delete p;
}
int write(); // TODO

void vUSBTask(void*) {
    std::array<uint8_t, MAX_BUFFER_SIZE> buff = { 0 };

    stdio_flush();

    while(true) {
        char inputCommand = getchar();
        CommandCode_t cmd = static_cast<CommandCode_t>(inputCommand);
        size_t len = commandLength(cmd);

        for(size_t i = 0; i < len; i++) {
            buff[i] = getchar();
        }

        switch(cmd) {
        case CMD_NOP:
            putchar(ACK);
            break;
        case CMD_QUERY_IFACE:
            printf("%c%c%c", ACK, 0x00, 0x01);
            break;
        case CMD_QUERY_COMMANDS:
            putchar(ACK);
            for(int i = 0; i < 32; i++) {
                putchar(0);
            }
            break;
        case CMD_QUERY_NAME:
            printf("%c%s", ACK, SERPROG_NAME);
            break;
        case CMD_QUERY_SERBUF:
            printf("%c%c%c", ACK, MAX_BUFFER_SIZE >> 8, MAX_BUFFER_SIZE);
            break;
        case CMD_QUERY_BUSTYPE:
            printf("%c%c", ACK, 0);
            break;
        case CMD_QUERY_OPBUF:
            putchar(NAK);
            break;
        case CMD_QUERY_WRN_MAX:
            putchar(NAK);
            break;
        case CMD_READ_BYTE: {
            uint32_t addr = littleEndian(buff.data(), ADDR_LEN);
            read(addr, 1);
            break;
        }
        case CMD_READ_NBYTES: {
            uint32_t addr = littleEndian(buff.data(), ADDR_LEN);
            uint32_t len = littleEndian(buff.data() + ADDR_LEN, N_LEN);

            read(addr, len);
            break;
        }
        case CMD_OP_INIT:
        case CMD_OP_WRITE_BYTE:
        case CMD_OP_WRITE_N:
        case CMD_OP_DELAY_US:
        case CMD_OP_EXECUTE:
        case CMD_SYNC_NOP:
        case CMD_QUERY_RDN_MAX:
        case CMD_SET_BUSTYPE:
        case CMD_OP_SPI_TRANSFER:
        case CMD_SET_SPI_FREQ:
        default:
            break;
        }
    }
}
