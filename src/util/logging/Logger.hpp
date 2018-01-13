#ifndef DIGGLER_UTIL_LOGGING_LOGGER_HPP
#define DIGGLER_UTIL_LOGGING_LOGGER_HPP

#include <string>

namespace diggler {
namespace Util {
namespace Logging {

enum LogLevel {
  Verbose,
  Debug,
  Info,
  Warning,
  Error,
  Failure
};

namespace LogLevels {
  constexpr LogLevel
    Verbose = LogLevel::Verbose,
    Debug = LogLevel::Debug,
    Info = LogLevel::Info,
    Warning = LogLevel::Warning,
    Error = LogLevel::Error,
    Failure = LogLevel::Failure,
    Default = LogLevel::Info;
}

/** \class Logger
 * @brief Base class to create log sinks
 */
class Logger {
public:
  virtual ~Logger();

  /**
   * @brief Returns the name of the Logger instance
   */
  virtual const char* getName() const = 0;

  /**
   * @brief Log a message
   *
   * @param message Message to log
   * @param lvl     Log level
   * @param tag     Log tag, identifying the log category / class emitting the message
   */
  virtual void log(const std::string &message, LogLevel lvl, const std::string &tag) = 0;
};

}
}
}

#endif /* DIGGLER_UTIL_LOGGING_LOGGER_HPP */
