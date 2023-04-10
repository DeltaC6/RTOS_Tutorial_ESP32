#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// QUEUE
static const uint8_t QUEUE_SIZE = 5;
static QueueHandle_t msgQueue;

void counter(void *param) {
    int item = 0;
    while(true) {
        if(xQueueReceive(msgQueue, (void *) &item, 0) == pdTRUE) {
            Serial.println(item);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Initializing the queue
    msgQueue = xQueueCreate(QUEUE_SIZE, sizeof(int));

    // Creating a new task
    xTaskCreatePinnedToCore(
        counter,
        "Counter Task",
        1024 * 1,
        NULL,
        1,
        NULL,
        app_cpu
    );
}

void loop() {
    static int num = 0;
    if(xQueueSend(msgQueue, (void *) &num, 10) != pdTRUE) {
        Serial.println("Queue full");
    }
    num++;
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
