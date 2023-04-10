// Task
// Use two task to work with serial data
// Task1 will listen for serial input from the user and create the string inside
// heap memory until newline is found. It will notify task2 that message is rdy
// Task2 will wait for notification from task1. After that it will print the
// string and free up the heap memory

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Task handles
static TaskHandle_t serialListener_h;
static TaskHandle_t serialOutput_h;

// Message
static char *msg = NULL;
static volatile bool msgFlag = false;

void serialListener(void *param) {
    while(true) {
        if(Serial.available() > 0) {
            // Taking user input from serial
            String buf = Serial.readStringUntil('\n');            

            if(!msgFlag) {
                // Creating heap memory
                int sz = buf.length() + 1;
                msg = (char *) pvPortMalloc(sz * sizeof(char));

                // If fails to create heap then reset
                configASSERT(msg);

                memcpy(msg, buf.c_str(), sz);

                // Notifying the other task
                msgFlag = true;
            }
        }
    }
}

void serialOut(void *param) {
    while(true) {
        // Wait for notification
        if(msgFlag) {
            // Print the heap memory
            Serial.println(msg);

            // Free the heap memory
            vPortFree(msg);

            // Clearing the flag
            msgFlag = false;
        }
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Creating serial listener task
    xTaskCreatePinnedToCore(
        serialListener,
        "Serial Listener",
        1024 * 1,
        NULL,
        1,
        &serialListener_h,
        app_cpu
    );

    // Creating serial output task
    xTaskCreatePinnedToCore(
        serialOut,
        "Serial out",
        1024 * 1,
        NULL,
        1,
        &serialOutput_h,
        app_cpu
    );

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {
}
