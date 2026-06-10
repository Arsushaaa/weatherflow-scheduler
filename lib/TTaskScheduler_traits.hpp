#pragma once

#include "my_type_traits.hpp"

namespace ts {

namespace details {

template <typename T>
class TFuture;

template <typename T>
struct IsTFuture {
   static constexpr bool value = false;
};

template <typename T>
struct IsTFuture<TFuture<T>> {
   static constexpr bool value = true;
};

template <typename T>
static constexpr bool IsTFuture_v = IsTFuture<T>::value;

template <typename T>
static constexpr bool IsTFutureLike_v = IsTFuture_v<ml::RemoveCVRef_t<T>>;

template <typename Func, typename = void>
struct HasFirstArg {
   static constexpr bool value = false;
};

template <typename Func>
struct HasFirstArg<Func, ml::Void_t<ml::GetFirstArg_t<Func>>> {
   static constexpr bool value = true;
};

template <typename Func>
static constexpr bool HasFirstArg_v = HasFirstArg<Func>::value;

template <typename Func>
using FirstArg_t = ml::GetFirstArg_t<Func>;

template <typename T>
struct NormalizeType {
   using Type = T;
};

template <typename T>
struct NormalizeType<T&> {
   using Type = T&;
};

template <typename T>
struct NormalizeType<T&&> {
   using Type = T;
};

template <typename T>
using NormalizeType_t = typename NormalizeType<T>::Type;

} // namespace detils

} // namespace ts