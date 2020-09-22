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
  microLAX = 1000000000000,
  milliLAX = 1000000000000000,
  LAX = 1000000000000000000,
  kLAX = LAX * 1000,
  mLAX = kLAX * 1000,
  gLAX = mLAX * 1000,
#else
  microLAT = 1000000000000,
  milliLAT = 1000000000000000,
  LAT = 1000000000000000000,
  kLAT = LAT * 1000,
  mLAT = kLAT * 1000,
  gLAT = mLAT * 1000,
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
 * @brief 1_microLAX representing 1e12 VON
 * @param mircolax Amount of microLAX
 * @return Energon
 *
 * Energon e = 1_microLAX;
 */
Energon operator""_microLAX(uint64_t microlax) {
  return Energon(u128(microlax) * u128(EnergonUnit::microLAX));
}

/**
 * string-literal operator
 *
 * @brief 1_milliLAX representing 1e15 VON
 * @param millilax Amount of milliLAX
 * @return Energon
 *
 * Energon e = 6_milliLAX;
 */
Energon operator""_milliLAX(uint64_t millilax) {
  return Energon(u128(millilax) * u128(EnergonUnit::milliLAX));
}

/**
 * string-literal operator
 *
 * @brief 1_LAX representing 1e18 Von
 * @param lax Amount of LAX
 * @return Energon
 *
 * Energon e = 33_LAX;
 */
Energon operator""_LAX(uint64_t lax) {
  return Energon(u128(lax) * u128(EnergonUnit::LAX));
}

/**
 * string-literal operator
 *
 * @brief 1_kLAX representing 1e21 VON
 * @param klax Aomount of kLAX
 * @return Energon
 *
 * Engergon e = 10_kLAX;
 */
Energon operator""_kLAX(uint64_t klax) {
  return Energon(u128(klax) * u128(EnergonUnit::kLAX));
}

/**
 * string-literal operator
 *
 * @brief 1_mLAX representing 1e24 VON
 * @param mlax Aomount of mLAX
 * @return Energon
 *
 * Engergon e = 10_mLAX;
 */
Energon operator""_mLAX(uint64_t mlax) {
  return Energon(u128(mlax) * u128(EnergonUnit::mLAX));
}

/**
 * string-literal operator
 *
 * @brief 1_gLAX representing 1e27 VON
 * @param glax Aomount of gLAX
 * @return Energon
 *
 * Engergon e = 10_gLAX;
 */
Energon operator""_gLAX(uint64_t glax) {
  return Energon(u128(glax) * u128(EnergonUnit::gLAX));
}
#else
/**
 * string-literal operator
 *
 * @brief 1_microLAT representing 1e12 VON
 * @param mircolat Amount of microLAT
 * @return Energon
 *
 * Energon e = 1_microLAT;
 */
Energon operator""_microLAT(uint64_t microlat) {
  return Energon(u128(microlat) * u128(EnergonUnit::microLAT));
}

/**
 * string-literal operator
 *
 * @brief 1_milliLAT representing 1e15 VON
 * @param millilat Amount of milliLAT
 * @return Energon
 *
 * Energon e = 6_milliLAT;
 */
Energon operator""_milliLAT(uint64_t millilat) {
  return Energon(u128(millilat) * u128(EnergonUnit::milliLAT));
}

/**
 * string-literal operator
 *
 * @brief 1_LAT representing 1e18 Von
 * @param lat Amount of LAT
 * @return Energon
 *
 * Energon e = 33_LAT;
 */
Energon operator""_LAT(uint64_t lat) {
  return Energon(u128(lat) * u128(EnergonUnit::LAT));
}

/**
 * string-literal operator
 *
 * @brief 1_kLAT representing 1e21 VON
 * @param klat Aomount of kLAT
 * @return Energon
 *
 * Engergon e = 10_kLAT;
 */
Energon operator""_kLAT(uint64_t klat) {
  return Energon(u128(klat) * u128(EnergonUnit::kLAT));
}

/**
 * string-literal operator
 *
 * @brief 1_mLAT representing 1e24 VON
 * @param mlat Aomount of mLAT
 * @return Energon
 *
 * Engergon e = 10_mLAT;
 */
Energon operator""_mLAT(uint64_t mlat) {
  return Energon(u128(mlat) * u128(EnergonUnit::mLAT));
}

/**
 * string-literal operator
 *
 * @brief 1_gLAT representing 1e27 VON
 * @param glat Aomount of gLAT
 * @return Energon
 *
 * Engergon e = 10_gLAT;
 */
Energon operator""_gLAT(uint64_t glat) {
  return Energon(u128(glat) * u128(EnergonUnit::gLAT));
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
 * Energon energon = 1_VON + 1_LAT;
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
