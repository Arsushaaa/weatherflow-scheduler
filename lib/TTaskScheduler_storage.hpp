#pragma once

#include <memory>

#include "my_utility.hpp"
#include "TTaskScheduler_traits.hpp"

namespace ts {

namespace details {

template <typename T>
class LvalueArgHolder {
private:
   T* value_;

public:
   explicit LvalueArgHolder(T& value)
      : value_(&value)
   {}

   T& get() {
      return *value_;
   }
};

template <typename T>
class RvalueArgHolder {
private:
   T value_;

public:
   template <typename U>
   RvalueArgHolder(U&& value)
      : value_(ml::Forward<U>(value))
   {}

   T&& get() {
      return ml::Move(value_);
   }
};

template <typename Future>
class FutureArgHolder {
private:
   Future future_;

public:
   template <typename U>
   FutureArgHolder(U&& future)
      : future_(ml::Forward<U>(future))
   {}

   decltype(auto) get() {
      return future_.get();
   }
};


template <typename T>
class ResultHolder {
private:
   std::unique_ptr<T> value_ = nullptr;

public:
   ResultHolder() = default;

   template <typename U>
   void store(U&& value) {
      value_ = std::make_unique<T>(ml::Forward<U>(value));
   }

   T* getPtr() {
      return value_.get();
   }

   const T* getPtr() const {
      return value_.get();
   }
};

template <typename T>
class ResultHolder<T&> {
private:
   T* ptr_ = nullptr;

public:
   ResultHolder() = default;

   void store(T& value) {
      ptr_ = &value;
   }

   T* getPtr() {
      return ptr_;
   }

   const T* getPtr() const {
      return ptr_;
   }
};

template <typename T>
decltype(auto) ResolveArg(T&& arg) {
   if constexpr (IsTFutureLike_v<T>) {
      return ml::Forward<T>(arg).get();
   } else {
      return ml::Forward<T>(arg);
   }
}

template <typename Arg>
struct StoredArg {
private:
   using CleanType = ml::RemoveReference_t<Arg>;
   using FutureType = ml::RemoveCVRef_t<Arg>;

public:
   using Type = ml::Conditional_t< IsTFutureLike_v<Arg>,
      FutureArgHolder<FutureType>, ml::Conditional_t<ml::IsLvalueReference_v<Arg>,
      LvalueArgHolder<CleanType>, RvalueArgHolder<CleanType>>>;
};

template <typename Arg>
using StoredArg_t = typename StoredArg<Arg>::Type;

template <typename Arg>
StoredArg_t<Arg> MakeStoredArg(Arg&& arg) {
   return StoredArg_t<Arg>(ml::Forward<Arg>(arg));
}

template <typename T>
using InputResolverArg_t = decltype(ResolveArg(ml::DeclVal<T>()));


} //namespace details

} // namespace ts