#pragma once

namespace ml {

template <typename T>
T&& DeclVal();

template <bool Cond, typename T = void>
struct EnableIf {};

template <typename T>
struct EnableIf<true, T> {
   using Type = T;
};

template <bool Cond, typename T = void>
using EnableIf_t = typename EnableIf<Cond, T>::Type;

template <typename T>
struct IsMemberFunctionPointer {
   static constexpr bool value = false;
};

template <typename Func, typename Class, typename... Args>
struct IsMemberFunctionPointer<Func(Class::*)(Args...)> {
   static constexpr bool value = true;
};

template <typename Func, typename Class, typename... Args>
struct IsMemberFunctionPointer<Func(Class::*)(Args...) const> {
   static constexpr bool value = true;
};

template <typename T>
constexpr bool IsMemberFunctionPointer_v = IsMemberFunctionPointer<T>::value;

template <typename T>
struct RemoveReference {
   using Type = T;
};

template <typename T>
struct RemoveReference<T&> {
   using Type = T;
};

template <typename T>
struct RemoveReference<T&&> {
   using Type = T;
};

template <typename T>
using RemoveReference_t = typename RemoveReference<T>::Type;

template <typename Func, typename... Args, 
   typename = EnableIf_t<!IsMemberFunctionPointer_v<RemoveReference_t<Func>>>>
decltype(auto) Invoke(Func&& func, Args&&... args) {
   return static_cast<Func&&>(func)(static_cast<Args&&>(args)...);
}

template <typename Method, typename Obj, typename... Args, 
   typename = EnableIf_t<IsMemberFunctionPointer_v<RemoveReference_t<Method>>>>
decltype(auto) Invoke(Method&& method, Obj&& obj, Args&&... args) {
   return (static_cast<Obj&&>(obj).*method)(static_cast<Args&&>(args)...);
}

template <typename Method, typename Obj, typename... Args, 
   typename = EnableIf_t<IsMemberFunctionPointer_v<RemoveReference_t<Method>>>>
decltype(auto) Invoke(Method&& method, Obj* obj, Args&&... args) {
   return (obj->*method)(static_cast<Args&&>(args)...);
}

template <typename Func, typename... Args>
struct InvokeResult {
   using Type = decltype(Invoke(DeclVal<Func>(), DeclVal<Args>()...));
};

template <typename Func, typename... Args>
using InvokeResult_t = typename InvokeResult<Func, Args...>::Type;

template <typename T> 
struct IsLvalueReference {
   static constexpr bool value = false;
};

template <typename T> 
struct IsLvalueReference<T&> {
   static constexpr bool value = true;
};

template <typename T>
constexpr bool IsLvalueReference_v = IsLvalueReference<T>::value;

template <typename T>
struct IsReference {
   static constexpr bool value = false;
};

template <typename T>
struct IsReference<T&> {
   static constexpr bool value = true;
};

template <typename T>
struct IsReference<T&&> {
   static constexpr bool value = true;
};

template <typename T>
constexpr bool IsReference_v = IsReference<T>::value;

template <typename T, typename U>
struct IsSame {
   static constexpr bool value = false;
};

template <typename T>
struct IsSame<T, T> {
   static constexpr bool value = true;
};

template <typename T, typename U>
constexpr bool IsSame_v = IsSame<T, U>::value;


template <typename T>
struct GetFirstArg;

template <typename Func, typename Arg, typename... Args>
struct GetFirstArg<Func(*)(Arg, Args...)> {
   using arg = Arg;
};

template <typename Func, typename Arg, typename... Args>
struct GetFirstArg<Func(Arg, Args...)> {
   using arg = Arg;
};

template <typename Func, class Class, typename Arg, typename... Args>
struct GetFirstArg<Func(Class::*)(Arg, Args...) const> {
   using arg = Arg;
};

template <typename Func, class Class, typename Arg, typename... Args>
struct GetFirstArg<Func(Class::*)(Arg, Args...)> {
   using arg = Arg;
};

template <typename Func>
struct GetFirstArg : GetFirstArg<decltype(&Func::operator())> {};

template <typename T>
using GetFirstArg_t = typename GetFirstArg<T>::arg;

template <bool Cond, typename T, typename F>
struct Conditional {
   using Type = T;
};

template <typename T, typename F>
struct Conditional<false, T, F> {
   using Type = F;
};

template <bool Cond, typename T, typename F>
using Conditional_t = typename Conditional<Cond, T, F>::Type;


template <typename T>
struct RemoveConst {
   using Type = T;
};

template <typename T>
struct RemoveConst<const T> {
   using Type = T;
};

template <typename T>
using RemoveConst_t = typename RemoveConst<T>::Type;


template <typename T>
struct RemoveVolatile {
   using Type = T;
};

template <typename T>
struct RemoveVolatile<volatile T> {
   using Type = T;
};

template <typename T>
using RemoveVolatile_t = typename RemoveVolatile<T>::Type;


template <typename T>
struct RemoveCV {
   using Type = RemoveConst_t<RemoveVolatile_t<T>>;
};

template <typename T>
using RemoveCV_t = typename RemoveCV<T>::Type;


template <typename T>
struct IsArray {
   static constexpr bool value = false;
};

template <typename T>
struct IsArray<T[]> {
   static constexpr bool value = true;
};

template <typename T, unsigned long long N>
struct IsArray<T[N]> {
   static constexpr bool value = true;
};

template <typename T>
constexpr bool IsArray_v = IsArray<T>::value;


template <typename T>
struct RemoveExtent {
   using Type = T;
};

template <typename T>
struct RemoveExtent<T[]> {
   using Type = T;
};

template <typename T, unsigned long long N>
struct RemoveExtent<T[N]> {
   using Type = T;
};

template <typename T>
using RemoveExtent_t = typename RemoveExtent<T>::Type;


template <typename T>
struct IsFunction {
   static constexpr bool value = false;
};

template <typename R, typename... Args>
struct IsFunction<R(Args...)> {
   static constexpr bool value = true;
};

template <typename R, typename... Args>
struct IsFunction<R(Args..., ...)> {
   static constexpr bool value = true;
};

template <typename T>
constexpr bool IsFunction_v = IsFunction<T>::value;


template <typename T>
struct Decay {
private:
   using U = RemoveReference_t<T>;

public:
   using Type = Conditional_t<IsArray_v<U>, 
   RemoveExtent_t<U>*, Conditional_t<IsFunction_v<U>, 
   U*, RemoveCV_t<U>>>;
};

template <typename T>
using Decay_t = typename Decay<T>::Type;

template <typename...>
using Void_t = void;

template <typename T>
struct RemoveCVRef {
   using Type = ml::RemoveCV_t<ml::RemoveReference_t<T>>;
};

template <typename T>
using RemoveCVRef_t = typename RemoveCVRef<T>::Type;

} // namespace ml
