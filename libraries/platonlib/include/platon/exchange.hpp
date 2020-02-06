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
  VonUnit = 1,
  GVonUnit = 1000000000,
  LATUnit = 1000000000000000000
};

/**
 * @brief Energo is a type of currency
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
 * @brief 1_Von representing one Von
 * @param von Amount of Von
 * @return Energon
 */
Energon operator""_Von(uint64_t von) { return Energon(von); }

/**
 * string-literal operator
 *
 * @breif 1_GVon representing 1e9Von
 * @param gvon Amount of GVon
 * @return Energon
 */
Energon operator""_GVon(long double gvon) {
  return Energon(gvon * u256(GVonUnit));
}

/**
 * string-literal operator
 *
 * @breif 1_GVon representing 1e9Von
 * @param gvon Amount of GVon
 * @return Energon
 */
Energon operator""_GVon(uint64_t gvon) {
  return Energon(gvon * u256(GVonUnit));
}

/**
 * string-literal operator
 *
 * @brief 1_LAT representing 1e18Von
 * @param lat Amount of LAT
 * @return Energon
 */
Energon operator""_LAT(long double lat) { return Energon(lat * u256(LATUnit)); }

/**
 * string-literal operator
 *
 * @brief 1_LAT representing 1e18Von
 * @param lat Amount of LAT
 * @return Energon
 */
Energon operator""_LAT(uint64_t lat) { return Energon(lat * u256(LATUnit)); }

/**
 * @brief Implements the addition operators for Energon
 *
 * @param a Amount of Von
 * @param b Amount of Von
 * @return Energon
 */
Energon operator+(const Energon& a, const Energon& b) {
  return Energon(0).Add(a).Add(b);
}

/**
 * @brief Transfer the amount of Energon to address
 *
 * @param addr Accounts address
 * @param amount The amount of Energon
 * @return The call succeeds or fails
 */
int32_t platon_transfer(const Address& addr, const Energon& amount) {
  auto bs = amount.Bytes();
  return ::platon_transfer(addr.data(), bs.data(), bs.size());
}

}  // namespace platon
