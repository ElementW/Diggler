#ifndef DIGGLER_WORLD_EMERGER_HPP
#define DIGGLER_WORLD_EMERGER_HPP

#include "../Chunk.hpp"
#include "../util/WorkerPool.hpp"

namespace diggler {
namespace world {

class Emerger final {
private:
  class Worker : public util::Worker<ChunkWeakRef> {
  public:
    using util::Worker<ChunkWeakRef>::Worker;

    ~Worker();

    void processTask(Task&&) final override;
  };

  class Pool : public util::WorkerPool<Worker> {
  public:
    inline void queue(const ChunkWeakRef &c) {
      addTask(c);
    }
  } pool;

public:
  inline void queue(const ChunkRef &c) {
    pool.queue(c);
  }
  inline void queue(const ChunkWeakRef &c) {
    pool.queue(c);
  }
};

}
}

#endif /* DIGGLER_WORLD_EMERGER_HPP */
