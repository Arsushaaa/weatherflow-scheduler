#include "TTaskScheduler.hpp"

#include <gtest/gtest.h>

#include <string>

TEST(TTaskSchedulerApplyTest, ApplyUsesResultOfPreviousTask) {
   ts::TTaskScheduler scheduler;
   auto base = scheduler.add([]() { return 10; });
   auto derived = base.apply([](int value) { return value + 5; });

   EXPECT_EQ(derived.getResultSync(), 15);
}

TEST(TTaskSchedulerApplyTest, ChainedApplyComputesInCorrectOrder) {
   ts::TTaskScheduler scheduler;
   auto t1 = scheduler.add([]() { return 2; });
   auto t2 = t1.apply([](int value) { return value * 10; });
   auto t3 = t2.apply([](int value) { return value - 3; });

   EXPECT_EQ(t3.getResultSync(), 17);
}

TEST(TTaskSchedulerApplyTest, ApplyPreservesConstReferenceInputSemantics) {
   ts::TTaskScheduler scheduler;
   auto producer = scheduler.add([]() { return 99; });
   auto consumer = producer.apply([](const int& value) { return value + 1; });

   EXPECT_EQ(consumer.getResultSync(), 100);
}

TEST(TTaskSchedulerApplyTest, ApplyWorksWithLvalueCallable) {
   ts::TTaskScheduler scheduler;
   auto base = scheduler.add([]() { return 4; });
   auto callable = [](int value) { return value * value; };

   auto squared = base.apply(callable);
   EXPECT_EQ(squared.getResultSync(), 16);
}

TEST(TTaskSchedulerApplyTest, ApplyWorksWithConstReferenceFirstArg) {
   ts::TTaskScheduler scheduler;
   auto base = scheduler.add([]() { return std::string("abc"); });

   auto size = base.apply([](const std::string& value) {
      return static_cast<int>(value.size());
   });

   EXPECT_EQ(size.getResultSync(), 3);
}

TEST(TTaskSchedulerApplyTest, ApplyWithFunctionPointerCallableWorks) {
   ts::TTaskScheduler scheduler;
   auto source = scheduler.add([]() { return 6; });

   auto multiply = +[](int value) { return value * 7; };
   auto result = source.apply(multiply);

   EXPECT_EQ(result.getResultSync(), 42);
}
