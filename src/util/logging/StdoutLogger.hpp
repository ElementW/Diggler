#ifndef DIGGLER_UTIL_LOGGING_STDOUTLOGGER_HPP
#define DIGGLER_UTIL_LOGGING_STDOUTLOGGER_HPP

#include <mutex>
#include <string>

#include "Logger.hpp"

namespace Diggler {
namespace Util {
namespace Logging {

/** \class StdoutLogger
 * @brief Logger that outputs to an ANSI/vt-100 console
 */
class StdoutLogger : public Logger {
protected:
  std::mutex mtx;

public:
  const char* getName() const;
  void log(const std::string &message, LogLevel lvl, const std::string &tag);
};

}
}
}

#endif /* DIGGLER_UTIL_LOGGING_STDOUTLOGGER_HPP */
