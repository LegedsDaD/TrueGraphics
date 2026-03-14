#include "truegraphics/state/State.h"

namespace truegraphics::state {

StateInt::StateInt(int value) : value_(value) {}

int StateInt::get() const { return value_; }

void StateInt::set(int value) {
  value_ = value;
  for (auto& observer : observers_) {
    observer(value_);
  }
}

void StateInt::subscribe(std::function<void(int)> callback) {
  observers_.push_back(std::move(callback));
}

StateFloat::StateFloat(double value) : value_(value) {}

double StateFloat::get() const { return value_; }

void StateFloat::set(double value) {
  value_ = value;
  for (auto& observer : observers_) {
    observer(value_);
  }
}

void StateFloat::subscribe(std::function<void(double)> callback) { observers_.push_back(std::move(callback)); }

StateBool::StateBool(bool value) : value_(value) {}

bool StateBool::get() const { return value_; }

void StateBool::set(bool value) {
  value_ = value;
  for (auto& observer : observers_) {
    observer(value_);
  }
}

void StateBool::subscribe(std::function<void(bool)> callback) { observers_.push_back(std::move(callback)); }

StateString::StateString(std::string value) : value_(std::move(value)) {}

const std::string& StateString::get() const { return value_; }

void StateString::set(std::string value) {
  value_ = std::move(value);
  for (auto& observer : observers_) {
    observer(value_);
  }
}

void StateString::subscribe(std::function<void(const std::string&)> callback) { observers_.push_back(std::move(callback)); }

}  // namespace truegraphics::state
