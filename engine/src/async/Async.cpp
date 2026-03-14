#include "truegraphics/async/Async.h"

#include <utility>

namespace truegraphics::async {

void AsyncExecutor::schedule(Task task) {
  jobs_.push_back(std::async(std::launch::async, [task = std::move(task)]() { task(); }));
}

void AsyncExecutor::wait_all() {
  for (auto& job : jobs_) {
    job.wait();
  }
  jobs_.clear();
}

}  // namespace truegraphics::async
