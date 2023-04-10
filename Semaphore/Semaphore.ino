#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const uint8_t NO_OF_TASK = 5;

// Message
typedef struct {
    char text[32];
    int len;
} Message;

// Semaphore
static SemaphoreHandle_t sem;

// Mutex
static SemaphoreHandle_t mutex;

// Task
void messageTask(void *param) {
    Message msg = *(Message *) param;   // This is witchcraft; BEWARE! (Its called typecasting)

    // Printing the context of message and securing the serial section to 
    // avoid overlap messages
    xSemaphoreTake(mutex, portMAX_DELAY);
    Serial.print("Received-> Message Text: ");
    Serial.print(msg.text);
    Serial.print(" Message Len: ");
    Serial.println(msg.len);
    xSemaphoreGive(mutex);

    // Now delete this task from memory after a small delay
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

// PRODUCER AND SUBSCRIBER MECHANISM
void setup() {
    Serial.begin(115200);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Message container
    Message msg[5];

    // Creating mutex for serial
    mutex = xSemaphoreCreateMutex();

    // Creating the semaphore
    sem = xSemaphoreCreateCounting(NO_OF_TASK, 0);

    // Creating the task
    char text[16];
    for(int i = 0; i < NO_OF_TASK; i++) {
        sprintf(text, "Task %i", i);

        strcpy(msg[i].text, text);
        msg[i].len = strlen(text);

        xTaskCreatePinnedToCore(
            messageTask,
            text,
            1024,
            (void *) &msg[i],
            1,
            NULL,
            app_cpu
        );
    }

    // Locking setup from exiting
    for(int i = 0; i < NO_OF_TASK; i++) {
        xSemaphoreTake(sem, portMAX_DELAY);
    }

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}
