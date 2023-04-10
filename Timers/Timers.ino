#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Timers
static TimerHandle_t oneShotTimer;
static TimerHandle_t autoReloadTimer;

// Callbacks
void timerCallback(TimerHandle_t xTimer) {
    Serial.print("Timer interrupt fired: ");

    if((uint32_t) pvTimerGetTimerID(xTimer) == 0) {
        Serial.println("Timer0");
    }

    if((uint32_t) pvTimerGetTimerID(xTimer) == 1) {
        Serial.println("Timer1");
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Creating timers
    oneShotTimer = xTimerCreate(
        "One Shot Timer",               // Name of timer
        1000 / portTICK_PERIOD_MS,      // Period of timer in ticks
        pdFALSE,                        // Auto reload
        (void *) 0,                     // Timer ID
        timerCallback                   // Timer callback function
    );

    autoReloadTimer = xTimerCreate(
        "Auto reload Timer",
        2000 / portTICK_PERIOD_MS,
        pdTRUE,
        (void *) 1,
        timerCallback
    );

    if(oneShotTimer != NULL && autoReloadTimer != NULL) {
        Serial.println("Starting timers");
        xTimerStart(oneShotTimer, portMAX_DELAY);
        xTimerStart(autoReloadTimer, portMAX_DELAY);
    }

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {
}
