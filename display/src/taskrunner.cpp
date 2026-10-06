#include "TaskRunner.h"

#include <ablib.h>

TaskRunner::TaskRunner()
    : xTaskQueue(nullptr),
      xWorkerHandle(nullptr) {
  xTaskQueue = xQueueCreate(QUEUE_SIZE, sizeof(std::function<void()>*));
  if (xTaskQueue == nullptr) {
    LOG("TaskRunner: cannot create task-queue.");
  }
}

TaskRunner::~TaskRunner() {
  if (xWorkerHandle != nullptr) {
    vTaskDelete(xWorkerHandle);
  }
  if (xTaskQueue != nullptr) {
    vQueueDelete(xTaskQueue);
  }
}

bool TaskRunner::setup() {
  if (xTaskQueue == nullptr)
    return false;

  BaseType_t result = xTaskCreatePinnedToCore(workerTask, "TaskRunnerWorker", 8192, this, 1, &xWorkerHandle, 0);

  return (result == pdPASS);
}

void TaskRunner::schedule(std::function<void()> task) {
  if (xTaskQueue == nullptr)
    return;

  auto* taskPtr = new std::function<void()>(std::move(task));

  if (xQueueSend(xTaskQueue, &taskPtr, pdMS_TO_TICKS(10)) != pdTRUE) {
    LOG("TaskRunner: queue overflow - discard task.");
    delete taskPtr;
  }
}

void TaskRunner::workerTask(void* pvParameters) {
  TaskRunner* _this = static_cast<TaskRunner*>(pvParameters);
  std::function<void()>* incomingTaskPtr = nullptr;
  while (true) {
    if (xQueueReceive(_this->xTaskQueue, &incomingTaskPtr, portMAX_DELAY) == pdTRUE) {
      if (incomingTaskPtr != nullptr) {
        (*incomingTaskPtr)();
        delete incomingTaskPtr;
      }
    }
  }
}
