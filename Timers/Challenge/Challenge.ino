// Task
// Create a task where user will take input from serial. While user is writing
// anything on serial the LED should turn on. If there is inactivity on serial 
// for 5 seconds then LED should turn off. Echo serial entries back to terminal

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"

// Timer
static const uint32_t LCD_BACKLIGHT_TIMER_INTERVAL = 5000;
static const uint8_t LCD_BACKLIGHT_PIN = 2;
static TimerHandle_t lcdBacklightTimer = NULL;

// Timer Callback
void timerCallback(TimerHandle_t xTimer) {

    if((uint32_t) pvTimerGetTimerID(xTimer) == 0) {
        // Turning the LED off
        Serial.println("LCD Backlight off");
        digitalWrite(LCD_BACKLIGHT_PIN, LOW);
    }
}

// Task
void serialListener(void *param) {
    while(true) {
        if(Serial.available() > 0) {
            // Turning on the LED
            digitalWrite(LCD_BACKLIGHT_PIN, HIGH);

            // Reading from serial
            char c = Serial.read();
            
            // Echoing back to terminal
            Serial.print(c);

            // Restarting timer in case if serial activity is done by user
            xTimerStart(lcdBacklightTimer, portMAX_DELAY);
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Initializing LCD backlight LED control pin
    pinMode(LCD_BACKLIGHT_PIN, OUTPUT);
    digitalWrite(LCD_BACKLIGHT_PIN, LOW);

    // Creating auto reload timer
    lcdBacklightTimer = xTimerCreate(
        "LCD Backlight Timer",
        LCD_BACKLIGHT_TIMER_INTERVAL / portTICK_PERIOD_MS,
        pdFALSE,
        (void *) 0,
        timerCallback
    );

    // Starting timer
    if(lcdBacklightTimer != NULL) {
        xTimerStart(lcdBacklightTimer, portMAX_DELAY);
    }

    // Creating serial listener task
    xTaskCreate(
        serialListener,
        "Serial Listener",
        1024,
        NULL,
        1,
        NULL
    );

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {
}
