#include "cmd.h"
#include <cstddef>
#include <cstdint>
#include <sys/types.h>

#define ADDR_LEN 3
#define N_LEN 3

constexpr size_t Frame::opCodeLength(const opcode_t& opcode) {
    switch(opcode) {
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

Frame::Frame(uint8_t _opc) {
    this->opcode = static_cast<opcode_t>(_opc);
    this->opcode_len = opCodeLength(opcode);
}

size_t Frame::decodeInstr(uint8_t* inst, size_t n) {
    switch(this->opcode) {
    case CMD_READ_BYTE:

        this->addr = littleEndian(inst, ADDR_LEN);
        this->len = 1;
        this->payload.resize(this->len);

        break;
    case CMD_READ_NBYTES:

        this->addr = littleEndian(inst, ADDR_LEN);
        this->len = littleEndian(inst + ADDR_LEN, N_LEN);

        this->payload.resize(this->len);

        break;
    case CMD_OP_WRITE_BYTE:

        this->addr = littleEndian(inst, ADDR_LEN);
        this->len = 1;

        this->payload.resize(this->len);

        this->payload[0] = littleEndian(inst + ADDR_LEN, 1);
        break;
    case CMD_OP_WRITE_N:

        this->addr = littleEndian(inst, ADDR_LEN);
        this->len = littleEndian(inst + ADDR_LEN, N_LEN);

        this->payload.resize(this->len);

        break;
    case CMD_OP_DELAY_US:

        this->len = 4;
        this->payload.resize(this->len);

        for(uint8_t& byte : payload) {
            byte = *inst;
            inst++;
        }

        break;
    case CMD_SET_BUSTYPE:
        this->len = 1;
        this->payload.resize(this->len);
        this->payload[0] = littleEndian(inst, 1);

        break;
    case CMD_OP_SPI_TRANSFER:
        // TODO
        break;

    case CMD_SET_SPI_FREQ:
        this->len = 4;
        this->payload.resize(this->len);

        for(uint8_t& byte : payload) {
            byte = *inst;
            inst++;
        }
        break;

    default:
        break;
    }

    return 0;
}
Frame::~Frame() {}

size_t Frame::getOpcodeLen() {
    return this->opcode_len;
}
