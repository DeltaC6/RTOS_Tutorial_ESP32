// Task
// Create 5 producers task and 2 consumer task
// THe producer task will write thrice to the circular buffer
// The consumer task will read the circular buffer and print the values
// The buffer is protected by mutex and semaphores. The semaphores will count
// no of filled slot and no of free slot individually
// The producer thread life is until it sends the values while the consumer
// thread life is continuous

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Using only single core for now
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Buffer
static const uint32_t NUM_OF_WRITES = 3;
static const uint32_t BUFFER_SIZE = 5;
static uint32_t buffer[BUFFER_SIZE];
static uint32_t head = 0;
static uint32_t tail = 0;

// Semaphore
static SemaphoreHandle_t sem_bin;
static SemaphoreHandle_t sem_filled;
static SemaphoreHandle_t sem_empty;
static SemaphoreHandle_t mutex;

// Tasks
static const uint32_t NUM_PROD_TASKS = 5;
static const uint32_t NUM_CONS_TASKS = 2;

void producer(void *param) {
    int num = *(int *) param;

    // Release binary semaphore
    xSemaphoreGive(sem_bin);

    // Filling shared buffer with task ID
    for(int i = 0; i < NUM_OF_WRITES; i++) {

        // Taking from empty semaphore since we are filling the buffer
        xSemaphoreTake(sem_empty, portMAX_DELAY);

        xSemaphoreTake(mutex, portMAX_DELAY);
        // This is a critical section since accessing shared global resource
        buffer[head] = num;
        head = (head + 1) % BUFFER_SIZE;
        xSemaphoreGive(mutex);

        // Giving away the filled semaphore showing a spot is filled to consumer
        xSemaphoreGive(sem_filled);
    }

    // Deleting task
    vTaskDelete(NULL);
}

void consumer(void *param) {
    int val;

    while(true) {

        // Taking from the filled semaphore since we need to consume
        xSemaphoreTake(sem_filled, portMAX_DELAY);

        xSemaphoreTake(mutex, portMAX_DELAY);
        // This is a critical section since accessing shared global resource
        val = buffer[tail];
        tail = (tail + 1) % BUFFER_SIZE;
        Serial.print("Value: ");
        Serial.println(val);
        xSemaphoreGive(mutex);

        // Giving away the empty semaphore showing a spot is empty again
        xSemaphoreGive(sem_empty);
    }

    // Observations:
    // If any other task is created dynamically mid application to be a producer
    // this consumer task will remain alive to read and consume the data.
    // Irrespective of how many producer task there may be in design there
    // should remain consumer task(s) to capture the data
    // This task will remain state until new data is added to some shared
    // resource and is shown by a filled space/semaphore
}

void setup() {
    Serial.begin(115200);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Create the semaphores
    sem_bin = xSemaphoreCreateBinary();
    // To help create task with their unique ID

    sem_filled = xSemaphoreCreateCounting(BUFFER_SIZE, 0);
    // Since buffer is free sem_filled is initialized using 0 as initial value

    sem_empty = xSemaphoreCreateCounting(BUFFER_SIZE, BUFFER_SIZE);
    // Since buffer is free sem_filled is initialized using BUFFER_SIZE to show
    // all the spaces are available

    // Creating mutex to protect the buffer during read/write operation & serial
    mutex = xSemaphoreCreateMutex();

    // Creating 5 producer task
    char taskName[16];
    for(int i = 0; i < NUM_PROD_TASKS; i++) {
        sprintf(taskName, "Producer %i", i);

        xTaskCreatePinnedToCore(
            producer,
            taskName,
            1024,
            (void *) &i,
            1,
            NULL,
            app_cpu
        );

        // Wait for values to get passed then exit setup
        xSemaphoreTake(sem_bin, portMAX_DELAY);
    }

    // Create 2 consumer task
    for(int i = 0; i < NUM_CONS_TASKS; i++) {
        sprintf(taskName, "Consumer %i", i);

        xTaskCreatePinnedToCore(
            consumer,
            taskName,
            1024,
            NULL,
            1,
            NULL,
            app_cpu
        );
    }

    // Deleting current task
    vTaskDelete(NULL);
}

void loop() {
}
