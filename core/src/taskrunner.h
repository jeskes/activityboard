#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include <functional>

class TaskRunner {
 public:
  TaskRunner();
  ~TaskRunner();

  bool setup();
  void schedule(std::function<void()> task);

 private:
  QueueHandle_t xTaskQueue;
  TaskHandle_t xWorkerHandle;
  static const int QUEUE_SIZE = 10;

  static void workerTask(void* pvParameters);
};
