#pragma once

#include <future>
#include <functional>
#include <vector>

namespace truegraphics::async {

class AsyncExecutor final {
 public:
  using Task = std::function<void()>;

  void schedule(Task task);
  void wait_all();

 private:
  std::vector<std::future<void>> jobs_;
};

}  // namespace truegraphics::async
