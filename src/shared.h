#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include <cstddef>
#include <cstdint>

#define TASK_IDLE 1
#define TASK_LOW 2
#define TASK_MEDIUM 3
#define TASK_HIGH 4
#define TASK_CRITICAL 5

#define SERPROG_NAME "RP2040serprog\n"
#define MAX_BUFFER_SIZE 256

struct Action_t {
    enum proceeding { Read, Write, Pin } proc;

    TaskHandle_t handle;

    int32_t addr;
    size_t len;
    uint8_t data[MAX_BUFFER_SIZE];
};

#define ACTION_QUEUE_LEN 128

extern QueueHandle_t ActionQueue;
extern TaskHandle_t USBHandle;

void vUSBTask(void*);
void vProgrammerTask(void*);
