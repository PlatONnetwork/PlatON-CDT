#pragma once
namespace platon {
namespace solidity {
namespace abi {
template <typename T>
struct IsSlice : std::false_type {};
template <typename... Args>
struct IsSlice<std::vector<Args...>> : std::true_type {};
template <typename T>
inline constexpr bool IsSliceV = IsSlice<T>::value;

template <typename T>
struct IsArray : std::false_type {};
template <typename T, std::size_t N>
struct IsArray<std::array<T, N>> : std::true_type {};
template <typename T>
inline constexpr bool IsArrayV = IsArray<T>::value;

template <typename T>
struct IsBytes : std::false_type {};
template <std::size_t N>
struct IsBytes<std::array<uint8_t, N>> : std::true_type {};
template <typename T>
inline constexpr bool IsBytesV = IsBytes<T>::value;

template <typename T>
struct IsString : std::false_type {};
template <>
struct IsString<std::string> : std::true_type {};
template <typename T>
inline constexpr bool IsStringV = IsString<T>::value;

template <typename T>
struct IsTuple : std::false_type {};
template <typename... Args>
struct IsTuple<std::tuple<Args...>> : std::true_type {};
template <typename T>
inline constexpr bool IsTupleV = IsTuple<T>::value;

template <typename T>
struct SupportSol {
 private:
  typedef int True;
  typedef char False;

  template <typename C>
  static True &test(decltype(&C::IsDynamicType));
  template <typename C>
  static False &test(...);

 public:
  static constexpr const bool value = sizeof(test<T>(0)) == sizeof(True);
};
template <typename T>
inline constexpr bool SupportSolV = SupportSol<T>::value;

template <typename T>
bool IsVariableInput(const T &t) {
  return IsSliceV<std::decay_t<decltype(t)>> ||
         IsStringV<std::decay_t<decltype(t)>> ||
         IsBytesV<std::decay_t<decltype(t)>>;
}

template <typename Tuple, std::size_t... Is>
bool EachTupleElementDynamicType(const Tuple &t, std::index_sequence<Is...>);
template <typename T>
size_t GetTypeSize(const T &t);
template <typename T>
bool IsDynamicType(const T &t);
template <typename T>
const T &GetElement(const T &t, size_t pos) {
  return t;
}

template <typename T, std::size_t N>
const T &GetElement(const std::array<T, N> &t, size_t pos) {
  return t[pos];
}

template <typename T>
const T &GetElement(const std::vector<T> &t, size_t pos) {
  return t[pos];
}

template <typename... Args>
bool IsDynamicTypeContainer(const std::tuple<Args...> &t) {
  return EachTupleElementDynamicType(t, std::index_sequence_for<Args...>{});
}

template <typename T, std::size_t N>
bool IsDynamicTypeContainer(const std::array<T, N> &t) {
  return IsDynamicType(t[0]);
}
template <typename T, typename std::enable_if_t<!SupportSolV<T>, bool> = true>
bool IsDynamicTypeContainer(const T &t) {
  return false;
}

template <typename T, typename std::enable_if_t<SupportSolV<T>, bool> = true>
bool IsDynamicTypeContainer(const T &t) {
  return t.IsDynamicType();
}

template <typename T>
bool IsDynamicType(const T &t) {
  if (IsTupleV<std::decay_t<decltype(t)>> ||
      SupportSolV<std::decay_t<decltype(t)>>) {
    return IsDynamicTypeContainer(t);
  }
  return IsSliceV<std::decay_t<decltype(t)>> ||
         IsStringV<std::decay_t<decltype(t)>> ||
         (IsArrayV<std::decay_t<decltype(t)>> && IsDynamicTypeContainer(t));
}

template <typename Tuple, std::size_t... Is>
bool EachTupleElementDynamicType(const Tuple &t, std::index_sequence<Is...>) {
  return (IsDynamicType(std::get<Is>(t)) || ...);
}

template <typename T>
size_t Elements(const T &t) {
  return 0;
}

template <typename T, std::size_t N>
size_t Elements(const std::array<T, N> &t) {
  return t.size();
}

template <typename T>
size_t Elements(const std::vector<T> &t) {
  return t.size();
}

template <typename Tuple, std::size_t... Is>
size_t ForEachTupleElementSize(const Tuple &t, std::index_sequence<Is...>) {
  return (GetTypeSize(std::get<Is>(t)) + ...);
}

template <typename T, typename std::enable_if_t<!SupportSolV<T>, bool> = true>
size_t ElementsTypeTotalSize(const T &t) {
  return 0;
}
template <typename... Args>
size_t ElementsTypeTotalSize(const std::tuple<Args...> &t) {
  return ForEachTupleElementSize(t, std::index_sequence_for<Args...>{});
}

template <typename T, typename std::enable_if_t<SupportSolV<T>, bool> = true>
size_t ElementsTypeTotalSize(const T &t) {
  return t.GetTypeSize();
}

template <typename T>
size_t GetTypeSize(const T &t) {
  if (IsArrayV<std::decay_t<decltype(t)>> && !IsDynamicTypeContainer(t)) {
    if (Elements(t) != 0 &&
        (IsArrayV<std::decay_t<decltype(GetElement(t, 0))>> ||
         IsTupleV<std::decay_t<decltype(GetElement(t, 0))>> ||
         SupportSolV<std::decay_t<decltype(GetElement(t, 0))>>)) {
      return Elements(t) * GetTypeSize(GetElement(t, 0));
    }
    return Elements(t) * 32;
  } else if ((IsTupleV<std::decay_t<decltype(t)>> ||
              SupportSolV<
                  std::decay_t<decltype(t)>>)&&!IsDynamicTypeContainer(t)) {
    return ElementsTypeTotalSize(t);
  }
  return 32;
}
}  // namespace abi
}  // namespace solidity
}  // namespace platon