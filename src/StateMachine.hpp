#ifndef DIGGLER_STATE_MACHINE_HPP
#define DIGGLER_STATE_MACHINE_HPP

#include <memory>

#include "State.hpp"

namespace Diggler {

class StateMachine {
public:
  virtual void setNextState(std::unique_ptr<State> &&next) = 0;
};

}

#endif /* DIGGLER_STATE_MACHINE_HPP */
