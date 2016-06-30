#ifndef SETTINGS_HPP
#define SETTINGS_HPP
#include <string>

namespace Diggler {

class Config {
private:
  

public:
  bool bloom;

  Config();
  void loadDefaults();
  void load(const std::string &path);
  void save(const std::string &path) const;
};

}

#endif