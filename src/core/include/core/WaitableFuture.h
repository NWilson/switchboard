#ifndef WAITABLE_FUTURE_H
#define WAITABLE_FUTURE_H

#include <experimental/coroutine>
#include <memory>
#include <mutex>
#include <optional>

//#include <future>
//static std::promise<int> promiseTest;
//static std::future<int> futureTest;
#include <cassert>
#include <iostream>

namespace core {
  struct Runnable {
    virtual ~Runnable() {}
    virtual void run() = 0;
  };
  using RunnablePtr = std::shared_ptr<Runnable>;
  void postRunnable(RunnablePtr xxx);
  void postRunnable(RunnablePtr xxx, std::chrono::time_point<std::chrono::steady_clock> when);

  template<typename T>
  class WaitableFuture {
  public:
    class Promise;
    using promise_type = Promise;
    using Handle = std::experimental::coroutine_handle<Promise>;

    class State : public Runnable {
    public:
      State() : h(nullptr) { std::cerr << "State ctor" << std::endl; }
      ~State() { std::cerr << "State dtor" << std::endl; }
      bool done() {
        std::unique_lock<std::mutex> l(mtx);
        return val || ex;
      }
      bool setSuspend(std::experimental::coroutine_handle<> h_) {
        std::unique_lock<std::mutex> l(mtx);
        if (val || ex) return false;
        h = h_;
        return true;
      }
      void clearSuspend() {
        std::unique_lock<std::mutex> l(mtx);
        h = nullptr;
      }
      T get() {
        std::unique_lock<std::mutex> l(mtx);
        assert(ex || val);
        if (ex) {
          auto ex_ = std::move(ex);
          std::rethrow_exception(ex_);
        }
        auto val_ = std::move(val);
        return std::move(*val_);
      }
      bool return_value(T val_) {
        std::unique_lock<std::mutex> l(mtx);
        assert(!val && !ex);
        std::cerr << "State set to " << val_ << std::endl;
        val = std::move(val_);
        return bool{h};
      }
      bool unhandled_exception() {
        std::unique_lock<std::mutex> l(mtx);
        assert(!val && !ex);
        std::cerr << "State set to exception" << std::endl;
        ex = std::current_exception();
        return bool{h};
      }

      virtual void run() {
        if (h) h.resume();
      }

    private:
      State(const State&) = delete;
      State(State&&) = delete;
      State& operator=(const State&) = delete;
      State& operator=(State&&) = delete;

      std::mutex mtx;
      std::optional<T> val;
      std::exception_ptr ex;
      std::experimental::coroutine_handle<> h;
    };

    class Promise {
    public:
      Promise() {
        std::cerr << "Promise ctor" << std::endl;
        state = std::make_shared<State>();
      }
      ~Promise() {
        std::cerr << "Promise dtor" << std::endl;
      }

      auto get_return_object() {
        std::cerr << "Creating future from promise" << std::endl;
        return WaitableFuture{state, Handle::from_promise(*this)};
      }
      auto initial_suspend() {
        std::cerr << "Promise initial_suspend" << std::endl;
        return std::experimental::suspend_always{}; // Lazy evaluation
      }
      auto return_value(T v) {
        std::cerr << "Promise return_value" << std::endl;
        if (state->return_value(std::move(v)))
          postRunnable(state);
        return std::experimental::suspend_never{};
      }
      auto final_suspend() {
        std::cerr << "Promise final_suspend" << std::endl;
        // XXX currently WaitableFuture destroys its child coroutine if there's a cleanup - but
        // if WaitableFuture *disowns* its coroutine through runAndDiscard, we need to clean up
        // here!
        return std::experimental::suspend_always{};
      }
      void unhandled_exception() {
        std::cerr << "Promise unhandled_exception" << std::endl;
        if (state->unhandled_exception())
          postRunnable(state);
      }

    private:
      std::shared_ptr<State> state;
    };

    WaitableFuture(const std::shared_ptr<State>& state_, Handle h_)
      : state(state_), h(h_) {
      std::cerr << "WaitableFuture ctor" << std::endl;
    }
    WaitableFuture(const WaitableFuture&) = delete;
    WaitableFuture(WaitableFuture&& other) { *this = std::move(other); }
    ~WaitableFuture() {
//      if (state) state->cancel();
      if (state) std::cerr << "WaitableFuture dtor" << std::endl;
      if (h) h.destroy();
    }

    WaitableFuture& operator=(const WaitableFuture&) = delete;
    WaitableFuture& operator=(WaitableFuture&& rhs) {
      state = std::exchange(rhs.state, nullptr);
      h = std::exchange(rhs.h, nullptr);
      return *this;
    }

    struct Waitable {
      Waitable(const std::shared_ptr<State>& state_) : state(state_) {}
      Waitable(const Waitable&) = delete;
      Waitable(Waitable&& other) : state(std::move(other.state)) {}
      ~Waitable() { if (state) state->clearSuspend(); }
      Waitable& operator=(const Waitable&) = delete;
      Waitable& operator=(Waitable&&) = delete;

      bool await_ready() {
        bool done = state->done();
        std::cerr << "Waitable await_ready = " << done << std::endl;
        return done;
      }
      bool await_suspend(std::experimental::coroutine_handle<> h) {
        bool suspended = state->setSuspend(h);
        std::cerr << "Waitable await_suspend" << suspended << std::endl;
        return suspended;
      }
      T await_resume() {
        try {
          T rv = state->get();
          std::cerr << "Waitable await_resume = " << rv << std::endl;
          return rv;
        } catch (...) {
          std::cerr << "Waitable await_resume (threw)" << std::endl;
          throw;
        }
      }
      std::shared_ptr<State> state;
    };

    Waitable operator co_await() { return wait(); }
    Waitable wait() {
      assert(h);
      std::cerr << "WaitableFuture wait" << std::endl;
      Handle h_ = std::exchange(h, nullptr);
      h_.resume();
      return Waitable{state};
    }
    void runAndDiscard() {
      assert(h);
      std::cerr << "WaitableFuture runAndDiscard" << std::endl;
      // XXX set it going, and delete after coroutine has completed
      //state->setDeleteOnDone();
      Handle h_ = std::exchange(h, nullptr);
      h_.resume();
    }
    //XXX waitFor, waitUntil

  private:
    std::shared_ptr<State> state;
    Handle h;
  };

}

#endif
