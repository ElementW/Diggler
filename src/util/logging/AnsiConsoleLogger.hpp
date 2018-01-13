#ifndef DIGGLER_UTIL_LOGGING_ANSI_CONSOLE_LOGGER_HPP
#define DIGGLER_UTIL_LOGGING_ANSI_CONSOLE_LOGGER_HPP

#include <mutex>
#include <string>

#include "Logger.hpp"

namespace diggler {
namespace Util {
namespace Logging {

/** \class AnsiConsoleLogger
 * @brief Logger that outputs to an ANSI/vt-100 console
 */
class AnsiConsoleLogger : public Logger {
protected:
  std::mutex mtx;

public:
  /**
   * @brief Enables colored output
   */
  bool enableColors;
  /**
   * @brief Enables colored backround on log tags
   */
  bool enableBackground;

  AnsiConsoleLogger();

  const char* getName() const;
  void log(const std::string &message, LogLevel lvl, const std::string &tag);
};

}
}
}

#endif /* DIGGLER_UTIL_LOGGING_ANSI_CONSOLE_LOGGER_HPP */
