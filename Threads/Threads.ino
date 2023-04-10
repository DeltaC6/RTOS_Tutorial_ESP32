#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins
static const uint8_t LED_PIN = 2;

void blink(void *param) {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    while(true) {
        digitalWrite(LED_PIN, !digitalRead(2));
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing...");

    xTaskCreatePinnedToCore(
        blink,
        "Blink Task",
        1024,
        NULL,
        1,
        NULL,
        app_cpu
    );
}

void loop() {
}
