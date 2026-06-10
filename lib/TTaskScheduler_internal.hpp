#pragma once

#include "my_type_traits.hpp"
#include "my_type_traits.hpp"
#include "my_tuple.hpp"
#include "TTaskScheduler_storage.hpp"

namespace ts {

class TTaskScheduler;

namespace details {

template <typename Result>
class TTask;

template <typename T>
class TFuture {
private:
   ts::TTaskScheduler* scheduler_;
   ml::size_t ind_;

public:
   TFuture(ts::TTaskScheduler* scheduler, ml::size_t ind) 
      : scheduler_(scheduler),
      ind_(ind)
   {}

   T get() const;
};

class TaskBase {
public:
   virtual void execute() = 0;
   virtual bool isExecuted() const = 0;
   virtual void* getResultPtr() = 0;
   virtual const void* getResultPtr() const = 0;

   virtual bool isMoved() const = 0;
   virtual void setMoved() = 0;

   virtual ~TaskBase() = default;
};

template <typename Func, typename Result, typename... Args>
class TaskModel : public TaskBase {
private:
   static_assert(!(ml::IsReference_v<Result> && !ml::IsLvalueReference_v<Result>),
      "task result T&& is not supported");

   Func func_;
   ml::Tuple<details::StoredArg_t<Args>...> args_;
   details::ResultHolder<Result> result_;
   bool executed_ = false;
   bool moved_ = false;

   template<ml::size_t... Ind>
   decltype(auto) call(ml::IndexSequence<Ind...>) {
      return ml::Invoke(func_, ml::Get<Ind>(args_).get()...);
   }

public:
   template <typename F>
   TaskModel(F&& func, Args&&... args) 
      : func_(ml::Forward<F>(func)),
      args_(details::MakeStoredArg<Args>(ml::Forward<Args>(args))...)
   {}

   void execute() override {
      if (executed_) {
         return;
      }

      result_.store(call(ml::IndexSequenceFor<Args...>{}));
      executed_ = true;
   }

   bool isExecuted() const override {
      return executed_;
   }

   void* getResultPtr() override {
      return result_.getPtr();
   }

   const void* getResultPtr() const override {
      return result_.getPtr();
   }

   bool isMoved() const override {
      return moved_;
   }
   
   void setMoved() override {
      moved_ = true;
   }
};

template <typename Func, typename... Args>
class TaskModel<Func, void, Args...> : public TaskBase {
private:
   Func func_;
   ml::Tuple<details::StoredArg_t<Args>...> args_;
   bool executed_ = false;

   template<ml::size_t... Ind>
   void call(ml::IndexSequence<Ind...>) {
      ml::Invoke(func_, ml::Get<Ind>(args_).get()...);
   }

public:
   template <typename F>
   TaskModel(F&& func, Args&&... args) 
      : func_(ml::Forward<F>(func)),
      args_(details::MakeStoredArg<Args>(ml::Forward<Args>(args))...)
   {}

   void execute() override {
      if (executed_) {
         return;
      }

      call(ml::IndexSequenceFor<Args...>{});
      executed_ = true;
   }

   bool isExecuted() const override {
      return executed_;
   }

   void* getResultPtr() override {
      return nullptr;
   }

   const void* getResultPtr() const override {
      return nullptr;
   }

   bool isMoved() const override {
      return false;
   }

   void setMoved() override {}
};

template <typename Result>
class TTask {
private:
   ts::TTaskScheduler* scheduler_;
   ml::size_t ind_;
   
public:
   TTask(ts::TTaskScheduler* ptr, ml::size_t ind)
      : scheduler_(ptr),
      ind_(ind)
   {}

   template <typename Func, typename... Args>
   auto apply(Func&& func, Args&&... args);

   decltype(auto) getResultSync();

   template <typename T>
   TFuture<T> getFutureResult();
};

} // namespace details

} // namespace ts