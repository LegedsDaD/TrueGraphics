#pragma once

#include <functional>
#include <string>
#include <vector>

namespace truegraphics::state {

class StateInt final {
 public:
  explicit StateInt(int value = 0);

  int get() const;
  void set(int value);
  void subscribe(std::function<void(int)> callback);

 private:
  int value_ = 0;
  std::vector<std::function<void(int)>> observers_;
};

class StateFloat final {
 public:
  explicit StateFloat(double value = 0.0);

  double get() const;
  void set(double value);
  void subscribe(std::function<void(double)> callback);

 private:
  double value_ = 0.0;
  std::vector<std::function<void(double)>> observers_;
};

class StateBool final {
 public:
  explicit StateBool(bool value = false);

  bool get() const;
  void set(bool value);
  void subscribe(std::function<void(bool)> callback);

 private:
  bool value_ = false;
  std::vector<std::function<void(bool)>> observers_;
};

class StateString final {
 public:
  explicit StateString(std::string value = "");

  const std::string& get() const;
  void set(std::string value);
  void subscribe(std::function<void(const std::string&)> callback);

 private:
  std::string value_;
  std::vector<std::function<void(const std::string&)>> observers_;
};

}  // namespace truegraphics::state
