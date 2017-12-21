#include <WaitableFuture.h>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

using namespace core;




struct Queue {
  void push(RunnablePtr r) {
    std::unique_lock<std::mutex> l(m);
    c.push_back(r);
    cond.notify_one();
  }
  RunnablePtr poll() {
    std::unique_lock<std::mutex> l(m);
    while (c.empty())
      cond.wait(l);
    auto r = std::move(c.front());
    c.pop_front();
    return r;
  }
  std::list<RunnablePtr> c;
  std::mutex m;
  std::condition_variable cond;
};

struct Scheduler {
  using Time = std::chrono::time_point<std::chrono::steady_clock>;
  using Task = std::pair<RunnablePtr, Time>;

  void schedule(RunnablePtr r, Time when) {
    std::unique_lock<std::mutex> l(m);
    c.push_back(Task(r, when));
    cond.notify_one();
  }
  void runScheduler() {
    std::unique_lock<std::mutex> l(m);
    while (true) {
      if (c.empty()) {
        cond.wait(l);
      } else if (std::chrono::steady_clock::now() >= c.front().second) {
        RunnablePtr r = c.front().first;
        c.pop_front();
        if (r) r->run();
      } else {
        cond.wait_until(l, c.front().second);
      }
    }
  }
  // XXX Ha! rubbish, doesn't even attempt to order the times properly...
  std::list<Task> c;
  std::mutex m;
  std::condition_variable cond;
};

core::WaitableFuture<int> w1() {
  std::cerr << "Entering w1" << std::endl;
  co_return 1;
}

core::WaitableFuture<int> w2() {
  std::cerr << "Entering w2" << std::endl;
  int rv1 = co_await w1();
  std::cerr << "w1 returned " << rv1 << std::endl;
  co_return 2;
}

Queue q;
void core::postRunnable(RunnablePtr r) {
  q.push(r);
}

Scheduler s;
void core::postRunnable(RunnablePtr r, std::chrono::time_point<std::chrono::steady_clock> when) {
  s.schedule(r, when);
}

int main() {
  std::thread t([] { s.runScheduler(); });
  w2().runAndDiscard();
  while (true) {
    q.poll()->run();
  }
}