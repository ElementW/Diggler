#include "Log.hpp"

#include "logging/AnsiConsoleLogger.hpp"

namespace Diggler {
namespace Util {

using namespace Logging;

std::unique_ptr<Logging::Logger> CurrentLogger;

void InitLogging() {
  CurrentLogger = std::make_unique<AnsiConsoleLogger>();
  // CurrentLogger = std::make_unique<StdoutLogger>();
}

LogInput Log() {
  return LogInput(*CurrentLogger, LogLevels::Default);
}

LogInput Log(LogLevel lvl) {
  return LogInput(*CurrentLogger, lvl);
}

LogInput Log(LogLevel lvl, const std::string &tag) {
  return LogInput(*CurrentLogger, lvl, tag);
}

}
}
