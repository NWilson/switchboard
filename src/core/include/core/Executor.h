#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <memory>

namespace core {
  class IoContext;

  class Executor {
  public:
    Executor(const Executor& other) noexcept;
    Executor(Executor&& other) noexcept;
    Executor& operator=(const Executor& other) noexcept;
    Executor& operator=(Executor&& other) noexcept;

    bool runningInThisThread() const noexcept;
    IoContext& context() const noexcept;

    // Calls synchronously if runningInThisThread() is true,
    // otherwise posts.
    template<class Func>
    void dispatch(Func&& f) const;
  
    template<class Func>
    void post(Func&& f) const;

    // Defer means that f will block until the current stack has finished
    // running (ie at the point when defer() is called, the calling code
    // is holding a mutex that Func requires) - hence the implementation
    // ought to put the Func on a thread-local queue rather than post it
    // to another thread, which will simply block for no good reason.
    template<class Func>
    void defer(Func&& f) const;

  private:

    class Function {
    public:
      template<typename F>
      explicit Function(F f)
        : impl(std::make_unique<Impl<F>>(std::move(f))) {}
      Function(Function&& other)
        : impl(std::exchange(other.impl, nullptr)) {}
      ~Function() { if (impl) { impl->destroy(); delete impl; } }

      void operator()()
      { if (impl) impl->invoke(impl.get()); }

    private:
      // Lightweight "virtual" object (with single method in "vtable")
      struct ImplBase {
        void (*invoke)(ImplBase*);
        void (*destroy)(ImplBase*);
      };

      // Template to build invocation thunks on the fly for each F
      template<typename F>
      static void invoke(ImplBase* i)
      { static_cast<Impl<F>*>(i)->f(); }
      template<typename F>
      static void destroy(ImplBase* i)
      { static_cast<Impl<F>*>(i)->f.~F(); }

      template<typename F>
      struct Impl : ImplBase {
        Impl(F f_) {
          new (f) F(std::move(f));
          invoke = &Function::invoke<F>;
          destroy = &Function::destroy<F>;
        }

        alignas(F) char f[sizeof(F)];
      };

      ImplBase* impl;
    };
  };
  bool operator==(const Executor& a, const Executor& b) noexcept;
  bool operator!=(const Executor& a, const Executor& b) noexcept;

}

#endif
