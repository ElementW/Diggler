#include "Emerger.hpp"

#include "../CaveGenerator.hpp"
#include "../util/Log.hpp"

namespace diggler {

using Util::Log;
using namespace Util::Logging::LogLevels;

namespace world {

Emerger::Worker::~Worker() {
}

void Emerger::Worker::processTask(Task &&cwr) {
  ChunkRef c = cwr.lock();
  if (!c) {  // Chunk was not referenced anymore
    return;
  }

  // TODO: loading
  auto genStart = std::chrono::high_resolution_clock::now();
  CaveGenerator::GenConf gc;
  CaveGenerator::Generate(c->getWorld(), gc, c);
  auto genEnd = std::chrono::high_resolution_clock::now();
  auto genDelta = std::chrono::duration_cast<std::chrono::milliseconds>(genEnd - genStart);
  glm::ivec3 cp = c->getWorldChunkPos();
  Log(Verbose, "Emerger") << "Map gen for " << c->getWorld()->id << '.' <<
    cp.x << ',' << cp.y << ',' << cp.z << " took " << genDelta.count() << "ms, thread #" << id;
}

}
}
