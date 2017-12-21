#include <core/ExecutionContext.h>

#include <core/Scheduler.h>

using namespace core;

ExecutionContext::ExecutionContext()
  : scheduler(std::make_unique<Scheduler>(*this))
{}

ExecutionContext::~ExecutionContext() {}

ExecutionContext::executor_type ExecutionContext::getExecutor()
{
  // XXX how does an Executor work, how is it copyable?
}


ExecutionContext::Service::~Service() {}
