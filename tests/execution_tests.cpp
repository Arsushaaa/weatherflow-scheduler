#include "TTaskScheduler.hpp"

#include <gtest/gtest.h>

TEST(TTaskSchedulerExecutionTest, VoidTaskWithFutureDependencyWorks) {
   ts::TTaskScheduler scheduler;
   auto base = scheduler.add([]() { return 7; });
   int observed = 0;

   auto sink = scheduler.add(
      [&observed](int value) { observed = value * 3; },
      base.getFutureResult<int>()
   );

   sink.getResultSync();
   EXPECT_EQ(observed, 21);
}

TEST(TTaskSchedulerExecutionTest, ExecuteAllRunsPendingTasks) {
   ts::TTaskScheduler scheduler;
   int counter = 0;

   scheduler.add([&counter]() { ++counter; });
   scheduler.add([&counter]() { ++counter; });

   scheduler.executeAll();
   EXPECT_EQ(counter, 2);
}

TEST(TTaskSchedulerExecutionTest, ExecuteAllDoesNotReRunExecutedTasks) {
   ts::TTaskScheduler scheduler;
   int counter = 0;

   auto task = scheduler.add([&counter]() {
      ++counter;
      return counter;
   });

   EXPECT_EQ(task.getResultSync(), 1);
   scheduler.executeAll();
   EXPECT_EQ(counter, 1);
}

TEST(TTaskSchedulerExecutionTest, ExecuteAllResolvesDependenciesTransitively) {
   ts::TTaskScheduler scheduler;

   auto t1 = scheduler.add([]() { return 3; });
   auto t2 = scheduler.add([](int x) { return x + 2; }, t1.getFutureResult<int>());
   auto t3 = scheduler.add([](int x) { return x * 5; }, t2.getFutureResult<int>());

   scheduler.executeAll();
   EXPECT_EQ(t3.getResultSync(), 25);
}

TEST(TTaskSchedulerExecutionTest, VoidTaskExecutesOnlyOnceOnRepeatedGetResultSync) {
   ts::TTaskScheduler scheduler;
   int counter = 0;

   auto task = scheduler.add([&counter]() { ++counter; });
   task.getResultSync();
   task.getResultSync();

   EXPECT_EQ(counter, 1);
}

TEST(TTaskSchedulerExecutionTest, ExecuteAllOnEmptySchedulerDoesNothing) {
   ts::TTaskScheduler scheduler;
   scheduler.executeAll();
   SUCCEED();
}
