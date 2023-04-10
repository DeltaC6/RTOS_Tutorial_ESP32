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
static SemaphoreHandle_t serialMutex;

// Timer
static hw_timer_t *timer = NULL;
static const uint32_t TIMER_PRE_SCALE = 80;
static const uint32_t TIMER_MAX_COUNT = 1000000;
static portMUX_TYPE spinLock = portMUX_INITIALIZER_UNLOCKED;

// Global
static volatile uint32_t counter = 0;
static volatile int16_t adcValue = 0;

// ISR
void IRAM_ATTR onTimer(void) {
    BaseType_t taskWoken = pdFALSE;

    // ESP IDF critical section
    portENTER_CRITICAL_ISR(&spinLock);
    counter++;
    adcValue = analogRead(A0);          // GPIO 36
    portEXIT_CRITICAL_ISR(&spinLock);

    xSemaphoreGiveFromISR(bin_sem, &taskWoken);

    // Vanilla FreeRTOS critical section
    // UBaseType_t saved_status;
    // saved_status = taskENTER_CRITICAL_FROM_ISR();
    // counter++;
    // taskEXIT_CRITICAL_FROM_ISR(saved_status);
    
    // Exit from ISR by yielding to a task
    if(taskWoken) {
        portYIELD_FROM_ISR();
    }

    // On vanilla  version of FreeRTOS
    // portYIELD_FROM_ISR(taskWoken);
}

// Task
void printValues(void *param) {
    while(true) {
        while(counter > 0) {
            xSemaphoreTake(serialMutex, portMAX_DELAY);
            Serial.print("Counter: ");
            Serial.println(counter);
            xSemaphoreGive(serialMutex);

            // Critical section inside task
            portENTER_CRITICAL(&spinLock);
            counter--;
            portEXIT_CRITICAL(&spinLock);

            // Vanilla FreeRTOS
            // taskENTER_CRITICAL();
            // counter--;
            // taskEXIT_CRITICAL();
        }

        // A small delay so the ISR can increment the variable
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void adcTask(void *param) {
    while(true) {
        xSemaphoreTake(bin_sem, portMAX_DELAY);

        xSemaphoreTake(serialMutex, portMAX_DELAY);
        Serial.print("ADC Val: ");
        Serial.println(adcValue);
        xSemaphoreGive(serialMutex);
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Initializing timer
    timer = timerBegin(0, TIMER_PRE_SCALE, true);
    timerAlarmWrite(timer, TIMER_MAX_COUNT, true);
    timerAttachInterrupt(timer, onTimer, true);
    timerAlarmEnable(timer);

    // Creating binary semaphore and mutex for serial
    bin_sem = xSemaphoreCreateBinary();
    serialMutex = xSemaphoreCreateMutex();

    // Creating task
    xTaskCreatePinnedToCore(
        printValues,
        "Printer",
        1024,
        NULL,
        1,
        NULL,
        app_cpu
    );

    xTaskCreatePinnedToCore(
        adcTask,
        "ADC task",
        1024,
        NULL,
        2,
        NULL,
        app_cpu
    );

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {
}
