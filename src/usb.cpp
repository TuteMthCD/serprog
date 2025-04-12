#include "FreeRTOSConfig.h"
#include "shared.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>


typedef enum {
    ACK = 0x06, // Acknowledge (positive response)
    NAK = 0x15, // Negative Acknowledge (negative response)

    CMD_NOP = 0x00,            // Do nothing
    CMD_QUERY_IFACE = 0x01,    // Query interface version
    CMD_QUERY_COMMANDS = 0x02, // Query supported commands
    CMD_QUERY_NAME = 0x03,     // Query programmer name
    CMD_QUERY_SERBUF = 0x04,   // Query serial buffer size
    CMD_QUERY_BUSTYPE = 0x05,  // Query supported bus types
    CMD_QUERY_CHIPSIZE = 0x06, // Query chip size (2^n format)
    CMD_QUERY_OPBUF = 0x07,    // Query operation buffer size
    CMD_QUERY_WRN_MAX = 0x08,  // Query max length for multi-byte write

    CMD_READ_BYTE = 0x09,   // Read one byte
    CMD_READ_NBYTES = 0x0A, // Read N bytes

    CMD_OP_INIT = 0x0B,       // Initialize operation buffer
    CMD_OP_WRITE_BYTE = 0x0C, // Write byte with address into buffer
    CMD_OP_WRITE_N = 0x0D,    // Write N bytes into buffer
    CMD_OP_DELAY_US = 0x0E,   // Add delay in microseconds
    CMD_OP_EXECUTE = 0x0F,    // Execute operation buffer

    CMD_SYNC_NOP = 0x10,      // Special NOP that responds with NAK+ACK
    CMD_QUERY_RDN_MAX = 0x11, // Query max length for multi-byte read

    CMD_SET_BUSTYPE = 0x12,     // Set bus type(s) to use
    CMD_OP_SPI_TRANSFER = 0x13, // Perform SPI operation
    CMD_SET_SPI_FREQ = 0x14,    // Set SPI clock frequency
    CMD_SET_PIN_STATE = 0x15    // Enable/disable output drivers
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
        return ADDR_LEN + N_LEN; // + data
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

void putArray(uint8_t* p, uint32_t len) {
    putchar(ACK);
    for(int i = 0; i < len; i++) {
        putchar(p[i]);
    }
}

void read(uint32_t addr, size_t len) {
    configASSERT(len > MAX_BUFFER_SIZE);
    Action_t* p = new Action_t();

    p->handle = xTaskGetCurrentTaskHandle();

    p->proc = Action_t::Read;
    p->addr = addr;
    p->len = len;

    xQueueSend(ActionQueue, &p, MAX_DELAY);
    size_t notify = xTaskNotifyWait(0x00, 0x00, NULL, MAX_DELAY);

    if(notify) {
        putArray(p->data, p->len);
    } else {
        putchar(NAK);
    }

    delete p;
}
void write(uint32_t addr, uint8_t* data, size_t len) {
    configASSERT(len > MAX_BUFFER_SIZE);
    Action_t* p = new Action_t();

    // p->handle = NULL; // if null, programmer delete action.
    p->handle = xTaskGetCurrentTaskHandle();

    p->proc = Action_t::Write;
    p->addr = addr;
    p->len = len;

    for(int i = 0; i < len; i++) {
        p->data[i] = data[i];
    }

    xQueueSend(ActionQueue, &p, MAX_DELAY);
    size_t notify = xTaskNotifyWait(0x00, 0x00, NULL, MAX_DELAY);

    if(notify) {
        putchar(ACK);
    } else {
        putchar(NAK);
    }

    // if(p->handle == NULL) return;
    delete p;
}

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
        case CMD_QUERY_IFACE: {
            uint8_t data[] = { ACK, 0x00, 0x01 };
            putArray(data, sizeof(data));
        } break;
        case CMD_QUERY_COMMANDS: {
            uint8_t data[32] = { 0 };
            putArray(data, sizeof(data));
        } break;
        case CMD_QUERY_NAME: {
            uint8_t name[] = SERPROG_NAME;
            putArray(name, sizeof(name));
        } break;
        case CMD_QUERY_SERBUF: {
            uint8_t data[] = { static_cast<uint8_t>(MAX_BUFFER_SIZE >> 8), static_cast<uint8_t>(MAX_BUFFER_SIZE) };
            putArray(data, sizeof(data));
        } break;
        case CMD_QUERY_BUSTYPE: {
            uint8_t data[1] = { 0 };
            putArray(data, 1);
        } break;
        case CMD_QUERY_OPBUF: {
            // uint8_t data[] = { static_cast<uint8_t>(ACTION_QUEUE_LEN >> 8), static_cast<uint8_t>(ACTION_QUEUE_LEN) };
            uint8_t data[] = { 0x00, 0x01 }; // QSYO AMIGO 1(UNO) no se que carajo me pedis, te lo dibujo
            putArray(data, sizeof(data));
        } break;
        case CMD_QUERY_WRN_MAX: {
            uint8_t data[] = { static_cast<uint8_t>(MAX_BUFFER_SIZE >> 8), static_cast<uint8_t>(MAX_BUFFER_SIZE) };
            putArray(data, sizeof(data));
        } break;
        case CMD_READ_BYTE: {
            uint32_t addr = littleEndian(buff.data(), ADDR_LEN);
            read(addr, 1);
        } break;
        case CMD_READ_NBYTES: {
            uint32_t addr = littleEndian(buff.data(), ADDR_LEN);
            uint32_t len = littleEndian(buff.data() + ADDR_LEN, N_LEN);

            read(addr, len);
        } break;
        case CMD_OP_INIT:
        case CMD_OP_WRITE_BYTE: {
            uint32_t addr = littleEndian(buff.data(), ADDR_LEN);
            write(addr, buff.data() + ADDR_LEN, 1);

        } break;
        case CMD_OP_WRITE_N: {
            uint32_t addr = littleEndian(buff.data(), ADDR_LEN);
            uint32_t len = littleEndian(buff.data() + ADDR_LEN, N_LEN);

            write(addr, buff.data() + ADDR_LEN + N_LEN, len);
        } break;

        case CMD_SYNC_NOP:
            putchar(NAK);
            putchar(ACK);
            break;

        case CMD_OP_DELAY_US:
        case CMD_OP_EXECUTE:
        case CMD_QUERY_RDN_MAX:
        case CMD_SET_BUSTYPE:
        case CMD_OP_SPI_TRANSFER:
        case CMD_SET_SPI_FREQ:
        default:
            putchar(NAK);
            break;
        }
    }
}
