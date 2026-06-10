#pragma once

#include "my_cstddef.hpp"
#include "my_utility.hpp"

namespace ml {

template <typename... Args>
struct Tuple;

template <>
struct Tuple<> {
   Tuple() = default;
};

template <typename Head, typename... Tail>
struct Tuple<Head, Tail...> {
   Head head_;
   Tuple<Tail...> tail_;

   Tuple() = default;
   template <typename H, typename... T>
   Tuple(H&& head, T&&... tail) 
      : head_(ml::Forward<H>(head)),
      tail_(ml::Forward<T>(tail)...)
   {}

};

template <size_t Ind, typename Tuple>
struct TupleElement;

template <typename Head, typename... Tail>
struct TupleElement<0, Tuple<Head, Tail...>> {
   using type = Head;
};

template <size_t Ind, typename Head, typename... Tail>
struct TupleElement<Ind, Tuple<Head, Tail...>> {
   using type = TupleElement<Ind-1, Tuple<Tail...>>::type;
};


template <size_t Ind, typename Head, typename... Tail>
decltype(auto) Get(Tuple<Head, Tail...>& tuple) {
   if constexpr (Ind == 0) {
      return (tuple.head_);
   } 
   else {
      return Get<Ind - 1>(tuple.tail_);
   }
}

template <size_t Ind, typename Head, typename... Tail>
decltype(auto) Get(const Tuple<Head, Tail...>& tuple) {
   if constexpr (Ind == 0) {
      return (tuple.head_);
   } 
   else {
      return Get<Ind - 1>(tuple.tail_);
   }
}

} // namespace ml