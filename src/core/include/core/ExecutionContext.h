#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

#include <core/Executor.h>
#include <memory>

namespace core {
  class CoarseScheduler;

  /// An ExecutionContext holds the relevant context for job management across
  /// an application, including the ability to obtain an Executor for
  /// dispatching jobs, and also management of common services in the
  /// application, such as a scheduler.

  class ExecutionContext {
  public:
    using executor_type = Executor;

    ExecutionContext();
    ExecutionContext(const ExecutionContext&) = delete;
    ExecutionContext& operator=(const ExecutionContext&) = delete;
    virtual ~ExecutionContext();

    /// @name Services
    /// @{

    class Service {
    protected:
      explicit Service(ExecutionContext& owner) : ctx(owner) {}
      Service(const Service&) = delete;
      Service& operator=(const Service&) = delete;
      virtual ~Service();
      
      ExecutionContext& context() noexcept { return ctx; }

    private:
      virtual void shutdown() = 0;

      ExecutionContext& ctx;
    };

    /// Obtains an Executor, which is a service that dispatches tasks via a
    /// pool of workers.
    Executor& getExecutor() { return *executor; }
    
    /// Returns the context's scheduler, for dispatching delayed jobs.
    CoarseScheduler& getcheduler() { return *scheduler; }

    //TODO: getIoService()
    //TODO: getDnsService()

    /// @}


    /// @name Lifecycle management
    /// @{

    /// shutdown() begins a clean shutdown of the executor, by setting a flag
    /// which will cause all open connections to eventually terminate cleanly
    /// (assuming that connections periodically check the shutdown flag).
    // XXX integrate properly with an application lifecycle system
    void shutdown();

    /// @}

  private:
    std::unique_ptr<Executor> executor;
    std::unique_ptr<CoarseScheduler> scheduler;
  };

}

#endif
