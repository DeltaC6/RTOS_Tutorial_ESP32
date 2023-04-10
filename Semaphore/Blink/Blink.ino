#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Semaphore
static SemaphoreHandle_t bin_sem;

// Task
void blinkTask(void *param) {
    static const uint8_t PIN_LED = 2;

    // Converting received value
    int delay = *(int *) param;       // Dark arts of C programming BEWARE!
    Serial.print("Received: ");
    Serial.println(delay);

    // Release semaphore
    xSemaphoreGive(bin_sem);

    // Pin settings
    pinMode(PIN_LED, OUTPUT);

    while(true) {
        digitalWrite(PIN_LED, !digitalRead(PIN_LED));
        vTaskDelay(delay / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Wait for input from user
    while(Serial.available() <= 0);

    int delayVal = Serial.parseInt();
    Serial.print("Sending delay value to blink task: ");
    Serial.println(delayVal);

    // Creating the semaphore
    bin_sem = xSemaphoreCreateBinary();

    // Creating the task
    xTaskCreatePinnedToCore(
        blinkTask,
        "Blink task",
        1024,
        (void *) &delayVal,
        1,
        NULL,
        app_cpu
    );

    // This will allow setup to wait until input is taken then exit
    xSemaphoreTake(bin_sem, portMAX_DELAY);

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {

    // Do nothing. Large delay is added to avoid Watch Dog reset
    vTaskDelay(60000 / portTICK_PERIOD_MS);
}
