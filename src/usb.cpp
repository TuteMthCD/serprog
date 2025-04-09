#include "shared.h"

#include "pico/stdio.h"
#include "pico/stdlib.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string.h>

#define MAX_BUFFER_SIZE 128

void vUSBTask(void*) {
    char* buff = (char*)malloc(sizeof(char) * MAX_BUFFER_SIZE);
    memset(buff, 0, MAX_BUFFER_SIZE);

    stdio_flush();

    int index = 0;
    int c = '\n';

    while(true) {
        while(index < MAX_BUFFER_SIZE) {
            c = getchar();
            buff[index++] = c;

            if(c == '\n') {
                break;
            }
        }

        CommandCode_t command = (CommandCode_t)buff[0];

        switch(command) {
        case CMD_NOP:
            putchar(ACK);
            break;

        case CMD_QUERY_IFACE:
            putchar(ACK);
            printf("%c%c", 0x00, 0x01);
            break;

        case CMD_QUERY_COMMANDS:
            putchar(ACK);
            for(int i = 0; i < 32; i++) {
                putchar(CMD_NOP);
            }
            break;

        case CMD_QUERY_NAME:
            printf("%c%16s", ACK, SERPROG_NAME);
            break;

        case CMD_QUERY_SERBUF:
            printf("%c%d", ACK, (uint16_t)MAX_BUFFER_SIZE);
            break;

        case CMD_QUERY_BUSTYPE:
            break;
        case CMD_QUERY_OPBUF:
            break;
        case CMD_QUERY_WRN_MAX:
            break;
        case CMD_READ_BYTE: {
            Action_t action;

            action.proc = Action_t::proceeding::Read;
            action.addr = buff[1] | buff[2] << 8 | buff[3] << 16;
            action.len = 1;

            xQueueSend(QActionQueue, &action, MAX_DELAY);

            uint8_t value;
            xQueueReceive(RActionQueue, &value, MAX_DELAY);

            putchar(value);
        } break;

        case CMD_READ_NBYTES:
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
            break;

        case ACK:
        case NAK:
            break;
        }

        memset(&buff, 0, index);
        index = 0;
    }
}
