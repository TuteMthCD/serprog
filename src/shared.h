#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <cstdint>

#define TASK_IDLE 1
#define TASK_LOW 2
#define TASK_MEDIUM 3
#define TASK_HIGH 4
#define TASK_CRITICAL 5

#define SERPROG_NAME "RP2040serprog"

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


struct Action_t {
    enum proceeding { Read, Write, Pin } proc;
    int32_t addr;
    int32_t len;
    uint8_t data[];
};

#define QACTION_QUEUE_LEN 128
static QueueHandle_t QActionQueue;

#define RACTION_QUEUE_LEN 256
static QueueHandle_t RActionQueue;

void vUSBTask(void*);
