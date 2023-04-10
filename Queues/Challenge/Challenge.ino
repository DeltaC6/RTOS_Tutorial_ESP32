// Task
// Work with two task doing following
// Task1: Prints any message for queue 2. Reads serial input from user. Echo 
// input back to serial. if "delay xxx" string is received then send xxx value
// to queue 1
// Task2: Updates time with new values form queue1. Blinks LED with time delay.
// Once LED blinks 100 times then send "Blinked" message to queue2
// (Optional) Also send no of times LED blinked.

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Queue
static const uint8_t QUEUE_SIZE = 10;
static QueueHandle_t messageQueue;      // Contains all string messages
static QueueHandle_t timeQueue;         // Contains all time values

// Message
static const uint32_t MAX_BLINK_COUNT = 100;
typedef struct {
    char msg[64];
    int count;
} Message;

// Tasks
void serialListener(void *param) {
    Message msg;
    char buffer[64];
    int idx = 0;

    while(true) {
        // Printing message from message queue
        if(xQueueReceive(messageQueue, (void *) &msg, 0) == pdTRUE) {
            Serial.print("Message: ");
            Serial.print(msg.msg);
            Serial.print(", Count: ");
            Serial.println(msg.count);
        }

        if(Serial.available() > 0) {
            // Read user input from serial
            char c = Serial.read();

            if(idx < 64 - 1) {
                buffer[idx++] = c;
            }
            
            if(buffer[idx - 1] == '\n') {
                buffer[idx - 1] = 0;
                Serial.println();

                // str = "some_random_characters_delay 1234\n"
                char *str = strstr(buffer, "delay ");
                if(str != 0) {
                    char *val = str + 6;
                    int delay = atoi(val);
                    delay = abs(delay);

                    // Sending the delay value to time queue
                    if(xQueueSend(timeQueue, (void *) &delay, 10) != pdTRUE) {
                        Serial.println("Time queue full");
                    }
                }

                // Clearing the buffer and index
                memset(buffer, 0, sizeof(buffer));
                idx = 0;
            } else {
                // Echo it back to serial
                Serial.print(c);
            }
        }
    }
}

void blinker(void *param) {
    Message msg;
    int delayInterval = 1000;
    int counter = 0;

    pinMode(2, OUTPUT);

    while(true) {
        // Check for new time interval from time queue
        if(xQueueReceive(timeQueue, (void *) &delayInterval, 0) == pdTRUE) {
            sprintf(msg.msg, "Delay time updated to: %d", delayInterval);
            msg.count = 1;
            xQueueSend(messageQueue, (prog_void *) &msg, 10);
        }

        // Blink led as per delay interval
        digitalWrite(2, !digitalRead(2));
        vTaskDelay(delayInterval / portTICK_PERIOD_MS);

        // Checking if the LED is blinked the no of required times
        counter++;
        if(counter >= MAX_BLINK_COUNT) {
            // Creating new message
            strcpy(msg.msg, "Blinked");
            msg.count = counter;

            // Sending the queue
            xQueueSend(messageQueue, (void *) &msg, 0);

            // Resetting counter
            counter = 0;
        }
    }
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Initializing the queues
    messageQueue = xQueueCreate(QUEUE_SIZE, sizeof(Message));
    timeQueue = xQueueCreate(QUEUE_SIZE, sizeof(int));

    // Creating the two tasks
    xTaskCreatePinnedToCore(
        serialListener,
        "Serial Task",
        1024,
        NULL,
        1,
        NULL,
        app_cpu
    );

    xTaskCreatePinnedToCore(
        blinker,
        "Blinker Task",
        1024,
        NULL,
        1,
        NULL,
        app_cpu
    );

    // Deleting the current task
    vTaskDelete(NULL);
}

void loop() {
}
