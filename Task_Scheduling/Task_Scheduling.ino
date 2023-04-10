#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Some message to print
static const char msg[] = "This is a test message string";

// Task handles
static TaskHandle_t hTask1 = NULL;
static TaskHandle_t hTask2 = NULL;

void task1(void *param) {
    int msg_len = strlen(msg);

    while(true) {
        Serial.println();
        for(int i = 0; i < msg_len; i++) {
            Serial.print(msg[i]);
        }
        Serial.println();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void task2(void *param) {
    while(true) {
        Serial.print("*");
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(300);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    
    // Print self priority
    Serial.print("Setup and loop task running on core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" with priority ");
    Serial.println(uxTaskPriorityGet(NULL));

    // Task to run
    xTaskCreatePinnedToCore(
        task1,
        "Task1",
        1024,
        NULL,
        1,
        &hTask1,
        app_cpu
    );

    xTaskCreatePinnedToCore(
        task2,
        "Task2",
        1024,
        NULL,
        2,
        &hTask2,
        app_cpu
    );
}

void loop() {
    // Suspend higher priority task
    for(int i = 0; i < 3; i++) {
        vTaskSuspend(hTask2);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        vTaskResume(hTask2);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    // Delete the lower priority task
    if(hTask1 != NULL) {
        vTaskDelete(hTask1);
        hTask1 = NULL;
    }
}
