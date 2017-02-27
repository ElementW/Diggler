#ifndef DIGGLER_UTIL_LOG_HPP
#define DIGGLER_UTIL_LOG_HPP

#include <memory>

#include "logging/LogInput.hpp"

namespace Diggler {
namespace Util {

extern std::unique_ptr<Logging::Logger> CurrentLogger;
void InitLogging();

Logging::LogInput Log();
Logging::LogInput Log(Logging::LogLevel);
Logging::LogInput Log(Logging::LogLevel, const std::string &tag);

}
}

#endif /* DIGGLER_UTIL_LOG_HPP */
