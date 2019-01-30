#ifndef DIGGLER_UTIL_WORKER_POOL
#define DIGGLER_UTIL_WORKER_POOL

#include <atomic>
#include <condition_variable>
#include <list>
#include <mutex>
#include <queue>
#include <type_traits>
#include <utility>

#include "Worker.hpp"

namespace diggler {
namespace util {

template<typename TaskT>
class WorkerPoolBase {
public:
  using Task = TaskT;

protected:
  friend Worker<Task>;

  std::queue<Task> taskQueue;
  std::recursive_mutex taskQueueMutex;
  std::condition_variable_any taskQueueCondVar;

  void addTask(const Task &task) {
    {
      std::unique_lock<std::recursive_mutex> lk(taskQueueMutex);
      taskQueue.push(task);
    }
    taskQueueCondVar.notify_one();
  }

  void addTask(Task &&task) {
    {
      std::unique_lock<std::recursive_mutex> lk(taskQueueMutex);
      taskQueue.push(std::move(task));
    }
    taskQueueCondVar.notify_one();
  }

  void startWorkerThread(Worker<Task> &worker) {
    if (!worker.thread.joinable()) {
      worker.thread = std::thread([](WorkerPoolBase *self, Worker<Task> &worker) {
        worker.run(*self);
      }, this, std::ref(worker));
    }
  }

  void markWorkerStop(Worker<Task> &worker) {
    worker.keepRunning.store(false, std::memory_order_release);
  }

  void stopWorkerThread(Worker<Task> &worker) {
    if (worker.thread.joinable()) {
      worker.thread.join();
    }
  }
};

template<class WorkerT, typename = std::enable_if_t<
    std::is_base_of<Worker<typename WorkerT::Task>, WorkerT>::value>>
class WorkerPool : public WorkerPoolBase<typename WorkerT::Task> {
public:
  using Task = typename WorkerT::Task;

protected:
  friend WorkerT;

  std::list<WorkerT> workers;

public:
  WorkerPool(unsigned nWorkers = 1) {
    for (unsigned i = 0; i < nWorkers; ++i) {
      workers.emplace_back(i);
      this->startWorkerThread(workers.back());
    }
  }

  ~WorkerPool() {
    for (WorkerT &worker : workers) {
      this->markWorkerStop(worker);
    }
    this->taskQueueCondVar.notify_all();
    for (WorkerT &worker : workers) {
      this->stopWorkerThread(worker);
    }
  }
};

}
}

#endif /* DIGGLER_UTIL_WORKER_POOL */
