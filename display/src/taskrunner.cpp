#include "TaskRunner.h"

void TaskRunner::setup() {
  queueMutex = xSemaphoreCreateMutex();
  /* worker function, name, stack-size, this-instance, priority, task-handle, core number */
  xTaskCreatePinnedToCore(TaskRunner::workerTask, "TaskWorker", 8192, this, 1, NULL, 0);
}

void TaskRunner::schedule(std::function<void()> task) {
  if (xSemaphoreTake(queueMutex, portMAX_DELAY) == pdTRUE) {
    taskQueue.push(task);
    xSemaphoreGive(queueMutex);
  }
}

void TaskRunner::workerTask(void* pvParameters) {
  TaskRunner* _this = (TaskRunner*)pvParameters;

  while (true) {
    std::function<void()> currentTask = nullptr;

    if (xSemaphoreTake(_this->queueMutex, pdMS_TO_TICKS(5)) == pdTRUE) {
      if (!_this->taskQueue.empty()) {
        currentTask = _this->taskQueue.front();
        _this->taskQueue.pop();
      }
      xSemaphoreGive(_this->queueMutex);
    }

    if (currentTask != nullptr) {
      currentTask();
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}
