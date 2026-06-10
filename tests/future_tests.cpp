#include "TTaskScheduler.hpp"

#include <gtest/gtest.h>

TEST(TTaskSchedulerFutureTest, ReferenceFutureCanBeReadManyTimes) {
   ts::TTaskScheduler scheduler;
   auto producer = scheduler.add([]() { return 21; });
   auto future = producer.getFutureResult<const int&>();

   const int& first = future.get();
   const int& second = future.get();

   EXPECT_EQ(first, 21);
   EXPECT_EQ(second, 21);
   EXPECT_EQ(&first, &second);
}

TEST(TTaskSchedulerFutureTest, MoveFutureCanBeReadOnlyOnce) {
   ts::TTaskScheduler scheduler;
   auto producer = scheduler.add([]() { return 33; });
   auto future = producer.getFutureResult<int>();

   EXPECT_EQ(future.get(), 33);
   EXPECT_THROW((void)future.get(), ml::Exception);
}

TEST(TTaskSchedulerFutureTest, FutureGetTriggersLazyExecution) {
   ts::TTaskScheduler scheduler;
   int counter = 0;

   auto producer = scheduler.add([&counter]() {
      ++counter;
      return 8;
   });

   auto future = producer.getFutureResult<int>();
   EXPECT_EQ(counter, 0);
   EXPECT_EQ(future.get(), 8);
   EXPECT_EQ(counter, 1);
}

TEST(TTaskSchedulerFutureTest, ConstFutureCanBePassedIntoAdd) {
   ts::TTaskScheduler scheduler;
   auto producer = scheduler.add([]() { return 10; });
   const auto future = producer.getFutureResult<int>();

   auto consumer = scheduler.add([](int value) { return value + 7; }, future);
   EXPECT_EQ(consumer.getResultSync(), 17);
}

TEST(TTaskSchedulerFutureTest, TwoConsumersUsingReferenceFutureSeeSameData) {
   ts::TTaskScheduler scheduler;
   auto producer = scheduler.add([]() { return 123; });
   auto shared = producer.getFutureResult<const int&>();

   auto c1 = scheduler.add([](const int& value) { return value + 1; }, shared);
   auto c2 = scheduler.add([](const int& value) { return value + 2; }, shared);

   EXPECT_EQ(c1.getResultSync(), 124);
   EXPECT_EQ(c2.getResultSync(), 125);
}

TEST(TTaskSchedulerFutureTest, ReferenceResultFutureReflectsUnderlyingObject) {
   ts::TTaskScheduler scheduler;
   int external = 5;

   auto producer = scheduler.add([&external]() -> int& { return external; });
   auto future = producer.getFutureResult<int&>();

   int& ref = future.get();
   ref += 10;

   EXPECT_EQ(external, 15);
   EXPECT_EQ(future.get(), 15);
}
