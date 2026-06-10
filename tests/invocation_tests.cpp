#include "TTaskScheduler.hpp"

#include <gtest/gtest.h>

TEST(TTaskSchedulerInvocationTest, AddSupportsMultipleArguments) {
   ts::TTaskScheduler scheduler;

   auto sum_task = scheduler.add(
      [](int a, int b, int c) { return a + b + c; },
      2,
      3,
      4
   );

   EXPECT_EQ(sum_task.getResultSync(), 9);
}

TEST(TTaskSchedulerInvocationTest, LvalueReferenceArgumentIsObservedByTask) {
   ts::TTaskScheduler scheduler;
   int value = 5;

   auto task = scheduler.add(
      [](int& ref) {
         ref += 10;
         return ref;
      },
      value
   );

   EXPECT_EQ(task.getResultSync(), 15);
   EXPECT_EQ(value, 15);
}

TEST(TTaskSchedulerInvocationTest, MemberFunctionPointerIsSupported) {
   struct Accumulator {
      int base;
      int add(int delta) {
         return base + delta;
      }
   };

   ts::TTaskScheduler scheduler;
   Accumulator accumulator{40};
   auto task = scheduler.add(&Accumulator::add, accumulator, 2);

   EXPECT_EQ(task.getResultSync(), 42);
}

TEST(TTaskSchedulerInvocationTest, MemberFunctionPointerOnObjectPointerIsSupported) {
   struct Multiplier {
      int mul(int x) const {
         return x * 3;
      }
   };

   ts::TTaskScheduler scheduler;
   Multiplier multiplier;
   auto task = scheduler.add(&Multiplier::mul, &multiplier, 14);

   EXPECT_EQ(task.getResultSync(), 42);
}

TEST(TTaskSchedulerInvocationTest, MemberFunctionCanMutateObjectState) {
   struct Counter {
      int value = 0;
      int bump(int delta) {
         value += delta;
         return value;
      }
   };

   ts::TTaskScheduler scheduler;
   Counter counter{};
   auto t1 = scheduler.add(&Counter::bump, counter, 3);
   auto t2 = scheduler.add(&Counter::bump, counter, 7);

   EXPECT_EQ(t1.getResultSync(), 3);
   EXPECT_EQ(t2.getResultSync(), 10);
}
