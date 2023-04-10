// Task:
// Use two task to control the blinking rate of LED
// 1st task listen to input from serial terminal
// 2nd task will toggle the LED as per the user input time delay

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// LED
static const uint8_t LED_PIN = 2;
uint32_t ledDelay = 1000;

void ledTask(void *param) {
    pinMode(LED_PIN, OUTPUT);
    
    while(true) {
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        vTaskDelay(ledDelay / portTICK_PERIOD_MS);
    }
}

void serialTask(void *param) {
    while(true) {
        if(Serial.available()) {
            String data = Serial.readString();
            ledDelay = atoi(data.c_str());

            Serial.print("Blink rate set to: ");
            Serial.print(ledDelay);
            Serial.println(" (ms)");
        }
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Print self priority
    Serial.print("Setup and loop task running on core ");
    Serial.print(xPortGetCoreID());
    Serial.print(" with priority ");
    Serial.println(uxTaskPriorityGet(NULL));

    xTaskCreate(
        ledTask,
        "LED task",
        1024,
        NULL,
        1,
        NULL
    );

    xTaskCreate(
        serialTask,
        "Serial Task",
        1024,
        NULL,
        2,
        NULL
    );
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
