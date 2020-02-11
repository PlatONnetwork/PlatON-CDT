#pragma once

#include "bigint.hpp"
#include "chain.hpp"
#include "common.h"
#include "fixedhash.hpp"

namespace platon {

/**
 * @brief The unit of Energon
 */
enum EnergonUnit {
  VON = 1,
  kVON = 1000,
  mVON = 1000000,
  gVON = 1000000000,
  uLAT = 1000000000000,
  mLAT = 1000000000000000,
  LAT = 1000000000000000000
};

/**
 * @brief Energo is a type of currency
 *
 * Example:
 *   void test() {
 *     // New a Energon
 *     Energon e(10);
 *     u256 von = 10;
 *     Energon o(u256);
 *
 *     // Gets value
 *     u256 val = e.Get();
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
  Energon(uint64_t n) : value_(n) {}

  /**
   * @brief Construct a new Energon
   *
   * @param n amount of Von
   */
  Energon(u256 n) : value_(n) {}

  /**
   * @brief Get amount of Von
   *
   * @return The amount of Von
   */
  const u256 Get() const { return value_; }

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
  Energon& Add(const u256& v) {
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
  u256 value_;
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
Energon operator""_kVON(uint64_t kvon) { return Energon(kvon * u256(kVON)); }

/**
 * @brief 1_mVON representing 1e6 VON
 *
 * @param mvon Amount of mVON
 * @return Energon
 *
 * Energon e = 5_mVON;
 */
Energon operator""_mVON(uint64_t mvon) { return Energon(mvon * u256(mVON)); }

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
Energon operator""_gVON(uint64_t gvon) { return Energon(gvon * u256(gVON)); }

/**
 * string-literal operator
 *
 * @brief 1_uLAT representing 1e12 VON
 * @param ulat Amount of uLAT
 * @return Energon
 *
 * Energon e = 1_uLAT;
 */
Energon operator""_uLAT(uint64_t ulat) { return Energon(ulat * u256(uLAT)); }

/**
 * string-literal operator
 *
 * @brief 1_mLAT representing 1e15 VON
 * @param mlat Amount of mLAT
 * @return Energon
 *
 * Energon e = 6_mLAT;
 */
Energon operator""_mLAT(uint64_t mlat) { return Energon(mlat * u256(mLAT)); }

/**
 * string-literal operator
 *
 * @brief 1_LAT representing 1e18Von
 * @param lat Amount of LAT
 * @return Energon
 *
 * Energon e = 33_LAT;
 */
Energon operator""_LAT(uint64_t lat) { return Energon(lat * u256(LAT)); }

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
    platon_throw("overflow");
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
