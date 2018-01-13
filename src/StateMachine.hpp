#ifndef DIGGLER_STATE_MACHINE_HPP
#define DIGGLER_STATE_MACHINE_HPP

#include <memory>

#include "states/State.hpp"

namespace diggler {

class StateMachine {
public:
  virtual ~StateMachine() {}

  virtual states::State& state() const = 0;
  virtual void setNextState(std::unique_ptr<states::State> &&next) = 0;
};

}

#endif /* DIGGLER_STATE_MACHINE_HPP */
