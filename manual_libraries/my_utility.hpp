#pragma once

#include "my_cstddef.hpp"
#include "my_type_traits.hpp"

namespace ml {

template <size_t... Indices>
struct IndexSequence {};

namespace details {

template <size_t N, size_t... Indices>
struct MakeIndexSequenceImpl
   : MakeIndexSequenceImpl<N - 1, N - 1, Indices...> 
{};

template <size_t... Indices>
struct MakeIndexSequenceImpl<0, Indices...> {
   using Type = IndexSequence<Indices...>;
};

} // namespace details

template <size_t N>
using MakeIndexSequence = typename details::MakeIndexSequenceImpl<N>::Type;

template <typename... Args>
using IndexSequenceFor = MakeIndexSequence<sizeof...(Args)>;

template <typename T>
constexpr RemoveReference_t<T>&& Move(T&& value) {
   return static_cast<RemoveReference_t<T>&&>(value);
}

template <typename T>
constexpr T&& Forward(RemoveReference_t<T>& value) {
   return static_cast<T&&>(value);
}

template <typename T>
constexpr T&& Forward(RemoveReference_t<T>&& value) {
   static_assert(!IsLvalueReference_v<T>);
   return static_cast<T&&>(value);
}

} // namespace ml