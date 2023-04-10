#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Shared resource
uint32_t shared_var = 0;

// Mutex
static SemaphoreHandle_t mutex;

// Tasks
void task(void *param) {
    int local_var = 0;

    while(true) {
        // Try to take the shared resource
        if(xSemaphoreTake(mutex, 0) == pdTRUE) {
            // Do some operation with shared resource
            local_var = shared_var;
            local_var++;
            shared_var = local_var;
            vTaskDelay(100 / portTICK_PERIOD_MS);

            // Printing the values
            Serial.print("Shared Var: ");
            Serial.println(shared_var);

            // Release the mutex
            xSemaphoreGive(mutex);
        } else {
            // Do something else
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Creating the mutex
    mutex = xSemaphoreCreateMutex();

    // Creating two threads accessing same shared resource
    xTaskCreatePinnedToCore(
        task,
        "Task1",
        1024,
        NULL,
        1,
        NULL,
        app_cpu
    );

    xTaskCreatePinnedToCore(
        task,
        "Task2",
        1024,
        NULL,
        1,
        NULL,
        app_cpu
    );

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {
}
