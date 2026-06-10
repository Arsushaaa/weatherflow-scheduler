#pragma once

#include <vector>
#include <memory>

#include "my_cstddef.hpp"
#include "my_type_traits.hpp"
#include "my_utility.hpp"
#include "my_exception.hpp"
#include "TTaskScheduler_internal.hpp"

namespace ts {

class TTaskScheduler {
private:
   template <typename Result>
   friend class details::TTask;
   template <typename T>
   friend class details::TFuture;

   std::vector<std::unique_ptr<details::TaskBase>> tasks_;

   template <typename Result>
   details::TTask<Result> getTask(ml::size_t ind) {
      return details::TTask<Result>(this, ind);
   }

public:
   TTaskScheduler() = default;
   TTaskScheduler (const TTaskScheduler& other) = delete;
   TTaskScheduler (TTaskScheduler&& other) = delete;
   TTaskScheduler& operator= (const TTaskScheduler& other) = delete;
   TTaskScheduler& operator= (TTaskScheduler&& other) = delete;

   template <typename Func, typename... Args>
   auto add(Func&& func, Args&&... args) {
      using Result = ml::InvokeResult_t<Func, details::InputResolverArg_t<Args>...>;
      using Model = details::TaskModel<ml::Decay_t<Func>, Result, Args...>;


      tasks_.push_back(std::make_unique<Model>(
         ml::Forward<Func>(func), ml::Forward<Args>(args)...));

      return details::TTask<Result>(this, tasks_.size() - 1);
   }

   void executeAll() {
      for (ml::size_t i = 0; i < tasks_.size(); ++i) {
         if (!tasks_[i]->isExecuted()) {
            tasks_[i]->execute();
         }
      }
   }
};

template <typename Result>
decltype(auto) details::TTask<Result>::getResultSync() {
   TaskBase* task = scheduler_->tasks_[ind_].get();
   task->execute();

   if constexpr (ml::IsSame_v<Result, void>) {
      return;
   }
   else if constexpr (ml::IsReference_v<Result>) {
      if (task->isMoved()) {
         throw ml::Exception("result was already moved");
      }

      return *static_cast<ml::RemoveReference_t<Result>*>(task->getResultPtr());
   } 
   else {
      if (task->isMoved()) {
         throw ml::Exception("result was already moved");
      }

      Result* ptr = static_cast<Result*>(task->getResultPtr());
      task->setMoved();
      return ml::Move(*ptr);
   }
}

template <typename Result>
template <typename Func, typename... Args> 
auto details::TTask<Result>::apply(Func&& func, Args&&... args) {
   using Callable = ml::RemoveReference_t<Func>;

   static_assert(details::HasFirstArg_v<Callable>,
      "apply does not support generic lambdas with auto as first argument");

   using FirstArg = details::FirstArg_t<Callable>;
   using FutureType = details::NormalizeType_t<FirstArg>;

   return scheduler_->add(
      ml::Forward<Func>(func),
      getFutureResult<FutureType>(),
      ml::Forward<Args>(args)...);
}

template <typename T>
T details::TFuture<T>::get() const {
   return scheduler_->template getTask<T>(ind_).getResultSync();
}

template <typename Result>
template <typename T>
details::TFuture<T> details::TTask<Result>::getFutureResult() {
   static_assert(!(ml::IsReference_v<T> && !ml::IsLvalueReference_v<T>),
      "TFuture<T&&> is not supported");
      
   static_assert(ml::IsSame_v<ml::Decay_t<T>, ml::Decay_t<Result>>);
   return details::TFuture<T>(scheduler_, ind_);
}

} // namespace ts