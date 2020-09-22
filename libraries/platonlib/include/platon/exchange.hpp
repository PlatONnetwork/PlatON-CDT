#pragma once

#include "chain.hpp"
#include "common.h"
#include "fixedhash.hpp"
#include "panic.hpp"

namespace platon {

/**
 * @brief The unit of Energon
 */
enum class EnergonUnit : u128 {
  VON = 1,
  kVON = 1000,
  mVON = 1000000,
  gVON = 1000000000,
#ifdef TESTNET
  microATX = 1000000000000,
  milliATX = 1000000000000000,
  ATX = 1000000000000000000,
  kATX = ATX * 1000,
  mATX = kATX * 1000,
  gATX = mATX * 1000,
#else
  microATP = 1000000000000,
  milliATP = 1000000000000000,
  ATP = 1000000000000000000,
  kATP = ATP * 1000,
  mATP = kATP * 1000,
  gATP = mATP * 1000,
#endif
};

/**
 * @brief Energo is a type of currency
 *
 * Example:
 *   void test() {
 *     // New a Energon
 *     Energon e(10);
 *     u128 von = 10;
 *     Energon o(u128);
 *
 *     // Gets value
 *     u128 val = e.Get();
 *
 *     // Serialize to big-ending bytes
 *     bytes bs = e.Bytes();
 *
 *     // Add amount of Energon
 *     Energon sum = e.Add(0);
 *     e += o;
 *   }
 */
class Energon {
 public:
  /**
   * @brief Construct a new Energon
   *
   * @param n amount of Von
   */
  Energon(u128 n) : value_(n) {}

  /**
   * @brief Get amount of Von
   *
   * @return The amount of Von
   */
  const u128 Get() const { return value_; }

  /**
   * @brief Get the bytes of value, the bytes use big-end representations
   *
   * @return The bytes of value
   */
  const bytes Bytes() const {
    bytes bs;
    bs.resize(32);
    toBigEndian(value_, bs);
    return bs;
  }

  /**
   * @brief Add amount of Von
   *
   * @param v Amount of Von
   * @return The reference of Energon
   */
  Energon& Add(const u128& v) {
    value_ += v;
    return *this;
  }

  /**
   * @brief Add amount of Von
   *
   * @param rhs Amount of Von
   * @return The reference of Energon
   */
  Energon& Add(const Energon& rhs) {
    this->value_ += rhs.value_;
    return *this;
  }

  /**
   * @brief Implement operator `+=`
   *
   * @param rhs Energon object
   * @return The reference of Energon
   */
  Energon& operator+=(const Energon& rhs) {
    this->value_ += rhs.value_;
    return *this;
  }

 private:
  u128 value_;
};

/**
 * string-literal operator
 *
 * @brief 1_VON representing one VON
 *
 * @param von Amount of VON
 * @return Energon
 *
 * Energon e = 100_VON;
 */
Energon operator""_VON(uint64_t von) { return Energon(von); }

/**
 * string-literal operator
 *
 * @brief 1_kVON representing 1000 VON
 *
 * @param kvon Amount of kVON
 * @return Energon
 *
 * Energon e = 10_kVON;
 */
Energon operator""_kVON(uint64_t kvon) {
  return Energon(u128(kvon) * u128(EnergonUnit::kVON));
}

/**
 * @brief 1_mVON representing 1e6 VON
 *
 * @param mvon Amount of mVON
 * @return Energon
 *
 * Energon e = 5_mVON;
 */
Energon operator""_mVON(uint64_t mvon) {
  return Energon(u128(mvon) * u128(EnergonUnit::mVON));
}

/**
 * string-literal operator
 *
 * @brief 1_GVon representing 1e9 VON
 *
 * @param gvon Amount of gVON
 * @return Energon
 *
 * Energon e = 2.1111_gVON;
 */
Energon operator""_gVON(uint64_t gvon) {
  return Energon(u128(gvon) * u128(EnergonUnit::gVON));
}

#ifdef TESTNET
/**
 * string-literal operator
 *
 * @brief 1_microATX representing 1e12 VON
 * @param mircoatx Amount of microATX
 * @return Energon
 *
 * Energon e = 1_microATX;
 */
Energon operator""_microATX(uint64_t microatx) {
  return Energon(u128(microatx) * u128(EnergonUnit::microATX));
}

/**
 * string-literal operator
 *
 * @brief 1_milliATX representing 1e15 VON
 * @param milliatx Amount of milliATX
 * @return Energon
 *
 * Energon e = 6_milliATX;
 */
Energon operator""_milliATX(uint64_t milliatx) {
  return Energon(u128(milliatx) * u128(EnergonUnit::milliATX));
}

/**
 * string-literal operator
 *
 * @brief 1_ATX representing 1e18 Von
 * @param atx Amount of ATX
 * @return Energon
 *
 * Energon e = 33_ATX;
 */
Energon operator""_ATX(uint64_t atx) {
  return Energon(u128(atx) * u128(EnergonUnit::ATX));
}

/**
 * string-literal operator
 *
 * @brief 1_kATX representing 1e21 VON
 * @param katx Aomount of kATX
 * @return Energon
 *
 * Engergon e = 10_kATX;
 */
Energon operator""_kATX(uint64_t katx) {
  return Energon(u128(katx) * u128(EnergonUnit::kATX));
}

/**
 * string-literal operator
 *
 * @brief 1_mATX representing 1e24 VON
 * @param matx Aomount of mATX
 * @return Energon
 *
 * Engergon e = 10_mATX;
 */
Energon operator""_mATX(uint64_t matx) {
  return Energon(u128(matx) * u128(EnergonUnit::mATX));
}

/**
 * string-literal operator
 *
 * @brief 1_gATX representing 1e27 VON
 * @param gatx Aomount of gATX
 * @return Energon
 *
 * Engergon e = 10_gATX;
 */
Energon operator""_gATX(uint64_t gatx) {
  return Energon(u128(gatx) * u128(EnergonUnit::gATX));
}
#else
/**
 * string-literal operator
 *
 * @brief 1_microATP representing 1e12 VON
 * @param mircoatp Amount of microATP
 * @return Energon
 *
 * Energon e = 1_microATP;
 */
Energon operator""_microATP(uint64_t microatp) {
  return Energon(u128(microatp) * u128(EnergonUnit::microATP));
}

/**
 * string-literal operator
 *
 * @brief 1_milliATP representing 1e15 VON
 * @param milliatp Amount of milliATP
 * @return Energon
 *
 * Energon e = 6_milliATP;
 */
Energon operator""_milliATP(uint64_t milliatp) {
  return Energon(u128(milliatp) * u128(EnergonUnit::milliATP));
}

/**
 * string-literal operator
 *
 * @brief 1_ATP representing 1e18 Von
 * @param atp Amount of ATP
 * @return Energon
 *
 * Energon e = 33_ATP;
 */
Energon operator""_ATP(uint64_t atp) {
  return Energon(u128(atp) * u128(EnergonUnit::ATP));
}

/**
 * string-literal operator
 *
 * @brief 1_kATP representing 1e21 VON
 * @param katp Aomount of kATP
 * @return Energon
 *
 * Engergon e = 10_kATP;
 */
Energon operator""_kATP(uint64_t katp) {
  return Energon(u128(katp) * u128(EnergonUnit::kATP));
}

/**
 * string-literal operator
 *
 * @brief 1_mATP representing 1e24 VON
 * @param matp Aomount of mATP
 * @return Energon
 *
 * Engergon e = 10_mATP;
 */
Energon operator""_mATP(uint64_t matp) {
  return Energon(u128(matp) * u128(EnergonUnit::mATP));
}

/**
 * string-literal operator
 *
 * @brief 1_gATP representing 1e27 VON
 * @param gatp Aomount of gATP
 * @return Energon
 *
 * Engergon e = 10_gATP;
 */
Energon operator""_gATP(uint64_t gatp) {
  return Energon(u128(gatp) * u128(EnergonUnit::gATP));
}
#endif

/**
 * @brief Compare two optional objects lhs and rhs is equal
 *
 * @param lhs Amount of VON
 * @param rhs Amount of VON
 * @return true if lhs == rhs, false otherwise
 *
 * Energon lhs(10);
 * Energon rhs(10);
 * if (lhs == rhs) {}
 */
bool operator==(const Energon& lhs, const Energon& rhs) {
  return lhs.Get() == rhs.Get();
}

/**
 * @brief Compare two optional objects lhs and rhs
 *
 * @param lhs Amount of VON
 * @param rhs Amount of VON
 * @return true if lhs > rhs, false otherwise
 *
 * Energon lhs(12);
 * Energon rhs(10);
 * if (lhs > rhs) {}
 */
bool operator>(const Energon& lhs, const Energon& rhs) {
  return lhs.Get() > rhs.Get();
}

/**
 * @brief Compare two optional objects lhs and rhs
 *
 * @param lhs Amount of VON
 * @param rhs Amount of VON
 * @return true if lhs < rhs, false otherwise
 *
 * Energon lhs(122);
 * Energon rhs(12222);
 * if (lhs < rhs) {}
 */
bool operator<(const Energon& lhs, const Energon& rhs) {
  return lhs.Get() < rhs.Get();
}

/**
 * @brief Implements the addition operators for Energon
 *
 * @param a Amount of VON
 * @param b Amount of VON
 * @return Energon
 *
 * Energon energon = 1_VON + 1_ATP;
 */
Energon operator+(const Energon& lhs, const Energon& rhs) {
  return Energon(0).Add(lhs).Add(rhs);
}

/**
 * @brief Implements the subtraction operator for Energon
 *
 * @param a Amount of VON
 * @param b Amount of VON
 * @return Energon
 *
 * Energon e = 1_kVON - 10_VON;
 * Energon e0(10);
 * Energon e1(2);
 * Energon ee = e0 - e1;
 */
Energon operator-(const Energon& lhs, const Energon& rhs) {
  if (lhs < rhs) {
    internal::platon_throw("overflow");
  }
  return Energon(lhs.Get() - rhs.Get());
}

/**
 * @brief Transfer the amount of Energon to address
 *
 * @param addr Accounts address
 * @param amount The amount of Energon
 * @return true if transfer success, false otherwise
 */
bool platon_transfer(const Address& addr, const Energon& amount) {
  auto bs = amount.Bytes();
  return ::platon_transfer(addr.data(), bs.data(), bs.size()) == 0;
}

}  // namespace platon
