#include <Arduino.h>

#include <functional>
#include <queue>

class TaskRunner {
 public:
  void setup();

  void schedule(std::function<void()> task);

 private:
  std::queue<std::function<void()>> taskQueue;
  SemaphoreHandle_t queueMutex;

  static void workerTask(void* pvParameters);
};
