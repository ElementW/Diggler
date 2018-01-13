#ifndef WORLD_HPP
#define WORLD_HPP
#include <map>
#include "World.hpp"

namespace diggler {

using UniverseWorldMap = std::map<WorldId, WorldWeakRef>;

class Universe : private UniverseWorldMap {
private:
  Game *G;

public:
  using UniverseWorldMap::size;
  using UniverseWorldMap::begin;
  using UniverseWorldMap::end;

  const bool isRemote;

  Universe(Game *G, bool remote);
  ~Universe();

  /**
   * @brief Gets a reference to a world.
   * Gets a world using its ID. If the world doesn't exist, returns an empty reference.
   * @returns Reference to the world.
   */
  WorldRef getWorld(WorldId id);

  /**
   * @brief Gets a reference to a world.
   * Gets a world using its ID. If the world doesn't exist in memory, creates it or
   * *synchronously* loads it.
   * @returns Reference to the world.
   */
  WorldRef getLoadWorld(WorldId id);

  /**
   * @brief Creates a world.
   * Creates a world with the specified ID. The World instance is constructed with
   * default parameters.
   * @note If a world with the same ID exists, an empty reference is returned.
   * @returns Reference to the newly created world.
   */
  WorldRef createWorld(WorldId id);

  /* ============ Serialization ============ */

  void write(io::OutStream&) const;
  void read(io::InStream&);

  void recv(net::InMessage&);
};

}

#endif
