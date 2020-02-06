#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void platon_debug(uint8_t *dst, size_t len);
void platon_revert(void);

#ifdef __cplusplus
}
#endif

namespace platon {
/**
 * @brief Platon assertion.
 *
 */
#define platon_assert(A, ...) \
  ::platon::assert_aux(A, #A, __LINE__, __FILE__, __func__, ##__VA_ARGS__)
/**
 * @brief Assertion A equals B.
 *
 */
#define platon_assert_eq(A, B, ...) platon_assert(((A) == (B)), ##__VA_ARGS__)
/**
 * @brief Assertion A not equal to B.
 */
#define platon_assert_ne(A, B, ...) platon_assert(((A) != (B)), ##__VA_ARGS__)

/**
 * @brief Platon assertion implementation, assertion failure output failure
 * location and error message.
 * @param cond      Assertion condition.
 * @param conndStr  Assertion prompt.
 * @param line      The line number of the code that failed the assertion.
 * @param file      Code file with assertion failure.
 * @param func      Function with assertion failure.
 * @param args      Argument list.
 */
template <typename... Args>
inline void assert_aux(bool cond, const char *condStr, unsigned line,
                       const char *file, const char *func, Args &&... args) {
  if (!cond) {
    println("Assertion failed:", condStr, "func:", func, "line:", line,
            "file:", file, args...);
    ::platon_revert();
  }
}
}  // namespace platon