#include "FreeRTOSConfig.h"
#include "cmd.h"
#include "shared.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>


void vUSBTask(void*) {
    std::array<uint8_t, MAX_BUFFER_SIZE> buff = { 0 };

    stdio_flush();

    while(true) {
        uint8_t opcode = static_cast<uint8_t>(getchar());
        Frame frame = Frame(opcode);
        size_t opcode_len = frame.getOpcodeLen();

        for(int i = 0; i < opcode_len; i++) {
            buff[i] = getchar();
        }

        frame.decodeInstr(buff.data(), opcode_len);
    }
}
