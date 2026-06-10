#include "TTaskScheduler.hpp"

#include <gtest/gtest.h>

namespace {

template <typename Task>
struct TaskResultType;

template <typename R>
struct TaskResultType<ts::details::TTask<R>> {
   using Type = R;
};

template <typename Task>
using TaskResultType_t = typename TaskResultType<Task>::Type;

} // namespace

TEST(TTaskSchedulerCompileTimeTest, AddReturnTypeMatchesCallableResult) {
   auto lambda = [](int x) { return x + 1; };
   using ReturnTask = decltype(ml::DeclVal<ts::TTaskScheduler&>().add(lambda, 1));

   static_assert(ml::IsSame_v<TaskResultType_t<ReturnTask>, int>);
   SUCCEED();
}

TEST(TTaskSchedulerCompileTimeTest, ApplyReturnTypeIsDerivedFromCallable) {
   using BaseTask = ts::details::TTask<int>;
   auto fn = [](const int& value) { return value * 2; };

   using NextTask = decltype(
      ml::DeclVal<BaseTask&>().apply(fn)
   );

   static_assert(ml::IsSame_v<TaskResultType_t<NextTask>, int>);
   SUCCEED();
}

TEST(TTaskSchedulerCompileTimeTest, NonGenericLambdaIsDetectedAsSupportedForApply) {
   auto non_generic = [](int x) { return x; };
   using Callable = ml::RemoveReference_t<decltype(non_generic)>;

   static_assert(ts::details::HasFirstArg_v<Callable>);
   SUCCEED();
}
