#ifndef DIGGLER_UTIL_WORKER
#define DIGGLER_UTIL_WORKER

#include <atomic>
#include <mutex>
#include <thread>

namespace diggler {
namespace util {

template<typename>
class WorkerPoolBase;

template<typename TaskT>
class Worker {
public:
  using WorkerPoolBaseT = WorkerPoolBase<TaskT>;
  friend WorkerPoolBaseT;

  using Task = TaskT;

private:
  std::atomic_bool keepRunning;

protected:
  unsigned id;
  std::thread thread;

  void run(WorkerPoolBaseT &pool) {
    keepRunning.store(true, std::memory_order_release);
    while (keepRunning.load(std::memory_order_relaxed)) {
      std::unique_lock<std::recursive_mutex> lk(pool.taskQueueMutex);
      if (pool.taskQueue.size() == 0) {
        // No more items to process, wait for more
        pool.taskQueueCondVar.wait(lk);
        if (!keepRunning.load(std::memory_order_relaxed))
          break;
        if (pool.taskQueue.size() == 0)  // Spurious wake-up
          continue;
      }
      Task task(std::move(pool.taskQueue.front()));
      pool.taskQueue.pop();
      lk.unlock();
      processTask(std::move(task));
    }
  }

public:
  Worker(unsigned id) :
    id(id) {
  }

  virtual ~Worker() {}

  virtual void processTask(Task&&) = 0;
};

}
}

#endif /* DIGGLER_UTIL_WORKER */
