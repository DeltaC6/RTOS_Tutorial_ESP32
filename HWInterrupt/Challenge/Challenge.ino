// Task
// Timer ISR sample data form ADC at 10Hz rate. After 10 samples are collected
// ISR will wake another task that will compute the average of those sample and
// store this value in global variable (float). Another task will handle all
// serial terminal commands. It will echo bach the char it sees. If a command
// 'AVG/avg' is entered then it will print the average value in global variable.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Timer
static hw_timer_t *timer = NULL;
static const uint32_t TIMER_PRE_SCALE = 80;         // 80MHz / 80 = 1MHz
static const uint32_t TIMER_MAX_COUNT = 100000;     // !MHz / 100K = 10Hz

// Adc daa
static const uint32_t MAX_SAMPLE_COUNT = 10;        // 10 sample / sec
static volatile float average = 0.0;

// Ring buffer
static const uint32_t BUFFER_SIZE = 15;
static int16_t buffer[BUFFER_SIZE] = {0};
static uint32_t head = 0;
static uint32_t tail = 0;

// Critical
static portMUX_TYPE spinLock = portMUX_INITIALIZER_UNLOCKED;

// Semaphore
static SemaphoreHandle_t bin_sem = NULL;

// ISR
void IRAM_ATTR onTimer(void) {
    static uint32_t sampleCounter = 0;
    BaseType_t taskWoken = pdFALSE;

    if(sampleCounter < MAX_SAMPLE_COUNT) {
        uint16_t val = analogRead(A0);

        // Put this value in ring buffer
        portENTER_CRITICAL_ISR(&spinLock);
        buffer[head] = val;
        head = (head + 1) % BUFFER_SIZE;
        portEXIT_CRITICAL_ISR(&spinLock);

        // Sample collected
        sampleCounter++;
    }

    // When all sample are collected then to wake up the task
    if(sampleCounter >= MAX_SAMPLE_COUNT) {
        // We can use suspend/resume task here for  average computation
        // taskWoken = xTaskResumeFromISR(&taskHandle);
        
        // We can use binary semaphore to notify the task to compute average
        xSemaphoreGiveFromISR(bin_sem, &taskWoken);

        // After waking up the task clear sample counter
        sampleCounter = 0;
    }

    if(taskWoken) {
        portYIELD_FROM_ISR();
    }
}

// Task
void serialTask(void *param) {
    static const uint32_t MAX_CMD_LEN = 32;
    char command[MAX_CMD_LEN] = {0};
    uint8_t idx = 0;

    while(true) {
        if(Serial.available() > 0) {
            // Read from serial
            char c = Serial.read();

            // Echo back the typed char
            Serial.print(c);

            // Fill the buffer
            if(idx < MAX_CMD_LEN) {
                command[idx++] = c;

                // Look for command
                if(c == '\n') {
                    command[idx] = 0;   // Terminate the buffer

                    char *found = strstr(command, "avg");
                    if(found != 0) {
                        Serial.print("Average: ");
                        Serial.println(average);
                    }

                    // Clear buffer and index;
                    memset(command, 0, MAX_CMD_LEN);
                    idx = 0;
                }
            } else {
                // Buffer full with garbage so clear it
                memset(command, 0, MAX_CMD_LEN);
                idx = 0;
            }
        }
    }
}

void adcTask(void *param) {
    while(true) {
        xSemaphoreTake(bin_sem, portMAX_DELAY);

        // Read the buffer and compute the average
        portENTER_CRITICAL(&spinLock);
        for(int i = 0; i < MAX_SAMPLE_COUNT; i++) {
            int16_t val = buffer[tail];       // BAD RING BUFFER E.G.
            tail = (tail + 1) % BUFFER_SIZE;  // Just an example implementation
            average += val;                   // of ring buffer
        }
        portEXIT_CRITICAL(&spinLock);

        average /= 10.0;
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Init timer
    timer = timerBegin(0, TIMER_PRE_SCALE, true);
    timerAlarmWrite(timer, TIMER_MAX_COUNT, true);
    timerAttachInterrupt(timer, onTimer, true);
    timerAlarmEnable(timer);

    // Creating semaphore
    bin_sem = xSemaphoreCreateBinary();

    // Creating task
    xTaskCreatePinnedToCore(
        serialTask,
        "Serial task",
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
        1,
        NULL,
        app_cpu
    );

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {
}
