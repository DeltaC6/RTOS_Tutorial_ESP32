/*
 *
 * Memory Allocation Schema
 * 
 * |============================|
 * |            RAM             |
 * |============================|
 * |                            |   Stack memory grows downwards when a function
 * |            Stack           |   is run in nested calls or recursively. When
 * |                            |   function goes out of scope stack is freed
 * |----------------------------|
 * |                            |
 * |                            |
 * |                            |
 * |                            |
 * |                            |
 * |                            |
 * |----------------------------|
 * |                            |   Heap memory grows upwards when a function
 * |            Heap            |   dynamically allocates a memory for some
 * |                            |   operation until it is freed
 * |----------------------------|
 * |                            |
 * |            Static          |   Fixed allocated memory (Global/Static)
 * |                            |
 * |----------------------------|
 * 
 */


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

void memoryTask(void *param) {
    while(true) {
        // Stack allocated to this task is 2048. Out of which 768 bytes are used
        // by task as overheads

        // Now allocating an integer array of size 100 which takes 400 bytes
        // in stack memory
        int arr[100];
        for(int i = 0; i < 100; i++) {
            arr[i] = i;
        }

        // Print remaining stack memory
        Serial.print("High Water Mark stack memory left (words): ");
        Serial.println(uxTaskGetStackHighWaterMark(NULL));

        // Allocate heap memory
        Serial.print("Heap memory before dynamic allocation: ");
        Serial.println(xPortGetFreeHeapSize());

        // Allocating heap memory dynamically
        // This will allocate heap memory and create an array of integer of size
        // 1024 which will take 4096 Bytes
        int *ptr = (int *) pvPortMalloc(1024 * sizeof(int));
        for(int i = 0; i < 1024; i++) {
            ptr[i] = i * 1000;
        }

        // Now checking remaining heap memory size
        Serial.print("Heap memory after dynamic allocation: ");
        Serial.println(xPortGetFreeHeapSize());


        // Now freeing the heap memory after operation
        vPortFree(ptr);

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(200 / portTICK_PERIOD_MS);

    // Creating new task
    xTaskCreatePinnedToCore(
        memoryTask,
        "Memory task",
        1024 * 2,
        NULL,
        1,
        NULL,
        app_cpu
    );

    // Deleting the current task
    vTaskDelete(NULL);
}

void loop() {
}
