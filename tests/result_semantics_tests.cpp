#include "TTaskScheduler.hpp"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>
#include <vector>

TEST(TTaskSchedulerResultSemanticsTest, MoveResultCanBeMovedOnlyOnce) {
   ts::TTaskScheduler scheduler;
   auto task = scheduler.add([]() {
      std::vector<int> values;
      values.push_back(1);
      values.push_back(2);
      values.push_back(3);
      return values;
   });

   auto moved = task.getResultSync();
   EXPECT_EQ(moved.size(), 3u);
   EXPECT_EQ(moved[0], 1);
   EXPECT_THROW((void)task.getResultSync(), ml::Exception);
}

TEST(TTaskSchedulerResultSemanticsTest, AddSupportsMoveOnlyArgument) {
   ts::TTaskScheduler scheduler;
   auto task = scheduler.add(
      [](std::unique_ptr<int> ptr) { return *ptr + 1; },
      std::make_unique<int>(41)
   );

   EXPECT_EQ(task.getResultSync(), 42);
}

TEST(TTaskSchedulerResultSemanticsTest, AddSupportsMoveOnlyResult) {
   ts::TTaskScheduler scheduler;
   auto task = scheduler.add([]() { return std::make_unique<int>(77); });

   auto ptr = task.getResultSync();
   ASSERT_NE(ptr, nullptr);
   EXPECT_EQ(*ptr, 77);
}

TEST(TTaskSchedulerResultSemanticsTest, TaskThatThrowsPropagatesException) {
   ts::TTaskScheduler scheduler;
   auto task = scheduler.add([]() -> int {
      throw std::runtime_error("boom");
   });

   EXPECT_THROW(task.getResultSync(), std::runtime_error);
}

TEST(TTaskSchedulerResultSemanticsTest, ReferenceResultCanBeReadManyTimes) {
   ts::TTaskScheduler scheduler;
   int storage = 11;
   auto task = scheduler.add([&storage]() -> int& {
      return storage;
   });

   int& first = task.getResultSync();
   int& second = task.getResultSync();
   first += 9;

   EXPECT_EQ(&first, &second);
   EXPECT_EQ(storage, 20);
}
