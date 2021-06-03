#pragma once

#include <span>
#include "platon/assert.hpp"
#include "platon/bigint.hpp"
#include "platon/chain.hpp"
namespace platon {
namespace crypto {
namespace bls12381 {

// Order is the number of elements in both G₁ and G₂.
std::uint256_t Order =
    "52435875175126190479447740508185965837690552500527637822603658699938581184513"_uint256;

// P is a prime over which we form a basic field.
std::uint384_t P =
    "4002409555221667393417789825735904156556882819939007885332058136124031650490837864442687629129015664037894272559787"_uint384;

std::uint256_t AddMod(const std::uint256_t& x, const std::uint256_t& y,
                      const std::uint256_t& k) {
  std::uint512_t sum = x;
  sum += y;
  return sum % k;
}

std::uint256_t SubMod(const std::uint256_t& x, const std::uint256_t& y,
                      const std::uint256_t& k) {
  if (x > y) {
    return AddMod(x - y, 0, Order);
  }
  std::uint512_t sub = x;
  sub += Order;
  sub -= y;
  return sub % k;
}

std::uint256_t MulMod(const std::uint256_t& x, const std::uint256_t& y,
                      const std::uint256_t& k) {
  std::uint512_t mul = x;
  mul *= y;
  return mul % k;
}

class G2;
class G1 {
 public:
  G1() : x_(0), y_(0) {}

  G1(const std::uint384_t& x, const std::uint384_t& y) : x_(x), y_(y) {}
  G1(const std::uint384_t&& x, const std::uint384_t&& y)
      : x_(std::move(x)), y_(std::move(y)) {}

  G1(const G1& g) : x_(g.x_), y_(g.y_) {}

  G1(const G1&& g) : x_(std::move(g.x_)), y_(std::move(g.y_)) {}

  G1& operator=(const G1& other) {
    this->x_ = other.x_;
    this->y_ = other.y_;
    return *this;
  }

  G1& operator=(const G1&& other) {
    this->x_ = std::move(other.x_);
    this->y_ = std::move(other.y_);
    return *this;
  }

  bool operator==(const G1& g1) {
    return this->x_ == g1.x_ && this->y_ == g1.y_;
  }

  /**
    * @brief to perform point addition on a crypto defined over prime field
    *
    * @param other point on a bls12381 crypto
    * @return -1 argument error, 0 success
    *
    * Example:
    *
    * @code
    * G1
    g1("3685416753713387016781088315183077757961620795782546409894578378688607592378376318836054947676345821548104185464507"_uint384,
        "1339506544944476473020471379941921221584933875938349620426543736416511423956333506472724655353366534992391756441569"_uint384);
    * G1
    g2("3685416753713387016781088315183077757961620795782546409894578378688607592378376318836054947676345821548104185464507"_uint384,
        "1339506544944476473020471379941921221584933875938349620426543736416511423956333506472724655353366534992391756441569"_uint384);
    * g1.AddStatus(g2);
    * @endcode
    *
    */
  int AddStatus(const G1& other) {
    return ::bls12381_g1_add(this->x_.Values(), this->y_.Values(),
                             other.x_.Values(), other.y_.Values(),
                             this->x_.Values(), this->y_.Values());
  }

  int AddStatus(const G1& l, const G1& r) {
    return ::bls12381_g1_add(l.x_.Values(), l.y_.Values(), r.x_.Values(),
                             r.y_.Values(), this->x_.Values(),
                             this->y_.Values());
  }
  /**
   * @brief to perform point addition on a crypto defined over prime field
   *
   * @param other point on a bls12381 crypto
   * @return object reference of self
   */
  G1& Add(const G1& other) {
    platon_assert(AddStatus(other) == 0);
    return *this;
  }

  G1& Add(const G1& l, const G1& r) {
    platon_assert(AddStatus(l, r) == 0);
    return *this;
  }

  /**
  * @brief  to perform point multiplication on a crypto defined over prime field
  *
  * @param scalar scalar value
  * @return -1 argument error, 0 success
  *
  * Example:
  *
  * @code
  * G1
  g1("3685416753713387016781088315183077757961620795782546409894578378688607592378376318836054947676345821548104185464507"_uint384,
      "1339506544944476473020471379941921221584933875938349620426543736416511423956333506472724655353366534992391756441569"_uint384);
  * G1
  g2("3685416753713387016781088315183077757961620795782546409894578378688607592378376318836054947676345821548104185464507"_uint384,
      "1339506544944476473020471379941921221584933875938349620426543736416511423956333506472724655353366534992391756441569"_uint384);
  * g1.ScalarMulStatus(g2);
  * @endcode
  *
  */
  int ScalarMulStatus(const std::uint256_t& scalar) {
    return ::bls12381_g1_mul(this->x_.Values(), this->y_.Values(),
                             scalar.Values(), this->x_.Values(),
                             this->y_.Values());
  }

  int ScalarMulStatus(const G1& g, const std::uint256_t& scalar) {
    return ::bls12381_g1_mul(g.x_.Values(), g.y_.Values(), scalar.Values(),
                             this->x_.Values(), this->y_.Values());
  }

  /**
   * @brief  to perform point multiplication on a crypto defined over prime
   * field
   *
   * @param scalar scalar value
   * @return object reference of self
   */
  G1& ScalarMul(const std::uint256_t& scalar) {
    platon_assert(ScalarMulStatus(scalar) == 0);
    return *this;
  }

  /**
   * @brief  to perform point multiplication on a crypto defined over prime
   * field
   *
   * @param g G1 point
   * @param scalar scalar value
   * @return object reference of self
   */
  G1& ScalarMul(const G1& g, const std::uint256_t& scalar) {
    platon_assert(ScalarMulStatus(g, scalar) == 0);
    return *this;
  }

  G1& Neg() {
    if (x_ == 0 && y_ == 0) {
      return *this;
    }
    y_ = P - (y_ % P);
    return *this;
  }

  /**
   * @brief  to perform multiexponentiation in G1
   *
   * @param g1 G1 point array
   * @param scalar scalar array
   * @return -1 argument error, 0 success
   */
  int MulExpStatus(const std::span<G1> g1,
                   const std::span<std::uint256_t> values) {
    size_t len = g1.size();
    size_t size = sizeof(uint8_t*) * len;
    const uint8_t** x1 = (const uint8_t**)malloc(size);
    const uint8_t** y1 = (const uint8_t**)malloc(size);
    const uint8_t** v = (const uint8_t**)malloc(size);

    for (size_t i = 0; i < len; i++) {
      x1[i] = g1[i].x_.Values();
      y1[i] = g1[i].y_.Values();
      v[i] = values[i].Values();
    }

    return ::bls12381_g1_mul_exp(x1, y1, v, len, this->x_.Values(), this->y_.Values());
  }

  /**
   * @brief  to perform multiexponentiation in G1
   *
   * @param g1 G1 point array
   * @param scalar scalar array
   * @return object reference of self
   */
  G1& MulExp(const std::span<G1> g1, const std::span<std::uint256_t> values) {
    platon_assert(MulExpStatus(g1, values) == 0);
    return *this;
  }

  /**
   * @brief  maps base field element into the G1 point
   *
   * @param fp field element
   * @return -1 argument error, 0 success
   */
  int MapStatus(const std::uint384_t& fp) {
    return ::bls12381_map_g1(fp.Values(), this->x_.Values(), this->y_.Values());
  }

  /**
   * @brief  maps base field element into the G1 point
   *
   * @param fp field element
   * @return object reference of self
   */
  G1& Map(const std::uint384_t& fp) {
    platon_assert(MapStatus(fp) == 0);
    return *this;
  }

  static G1 Base() {
    return G1(
        "3685416753713387016781088315183077757961620795782546409894578378688607592378376318836054947676345821548104185464507"_uint384,
        "1339506544944476473020471379941921221584933875938349620426543736416511423956333506472724655353366534992391756441569"_uint384);
  }

  static G1 ScalarBaseMul(const std::uint256_t& a) {
    G1 g = Base();
    g.ScalarMul(a);
    return g;
  }

  static G1 MulExpG1(const std::span<G1> g1,
                     const std::span<std::uint256_t> values) {
    G1 r;
    r.MulExp(g1, values);
    return r;
  }

  static G1 MapG1(const std::uint384_t& fp) {
    G1 r;
    r.Map(fp);
    return r;
  }

  std::uint384_t& X() { return x_; }
  const std::uint384_t& X() const { return x_; }
  std::uint384_t& Y() { return y_; }
  const std::uint384_t& Y() const { return y_; }
  friend int pairing(const std::span<G1> g1, const std::span<G2> g2);

 private:
  std::uint384_t x_;
  std::uint384_t y_;
};

class G2 {
 public:
  G2() : G2(0, 0, 0, 0) {}
  G2(const std::uint384_t& x1, const std::uint384_t& x2,
     const std::uint384_t& y1, const std::uint384_t& y2) {
    x_[0] = x1;
    y_[0] = y1;
    x_[1] = x2;
    y_[1] = y2;
  }

  G2(const G2& other) {
    x_[0] = other.x_[0];
    y_[0] = other.y_[0];
    x_[1] = other.x_[1];
    y_[1] = other.y_[1];
  }

  G2(const G2&& other) {
    x_[0] = std::move(other.x_[0]);
    y_[0] = std::move(other.y_[0]);
    x_[1] = std::move(other.x_[1]);
    y_[1] = std::move(other.y_[1]);
  }

  G2& operator=(const G2& other) {
    x_[0] = other.x_[0];
    y_[0] = other.y_[0];
    x_[1] = other.x_[1];
    y_[1] = other.y_[1];
    return *this;
  }

  G2& operator=(const G2&& other) {
    x_[0] = std::move(other.x_[0]);
    y_[0] = std::move(other.y_[0]);
    x_[1] = std::move(other.x_[1]);
    y_[1] = std::move(other.y_[1]);
    return *this;
  }

  bool operator==(const G2& other) {
    return x_[0] == other.x_[0] && y_[0] == other.y_[0] &&
           x_[1] == other.x_[1] && y_[1] == other.y_[1];
  }

  /**
  * @brief to perform point addition on a crypto twist defined over quadratic
  extension of the base field
  *
  * @param other twist point on a bls12381 crypto
  * @return -1 argument error, 0 success
  *
  * Example:
  *
  * @code
  * G2
  g1("3059144344244213709971259814753781636986470325476647558659373206291635324768958432433509563104347017837885763365758"_uint384,
  *
  "352701069587466618187139116011060144890029952792775240219908644239793785735715026873347600343865175952761926303160"_uint384,
  *
  "927553665492332455747201965776037880757740193453592970025027978793976877002675564980949289727957565575433344219582"_uint384,
  *
  "1985150602287291935568054521177171638300868978215655730859378665066344726373823718423869104263333984641494340347905"_uint384);
  * G2
  g2("3059144344244213709971259814753781636986470325476647558659373206291635324768958432433509563104347017837885763365758"_uint384,
        "352701069587466618187139116011060144890029952792775240219908644239793785735715026873347600343865175952761926303160"_uint384,
        "927553665492332455747201965776037880757740193453592970025027978793976877002675564980949289727957565575433344219582"_uint384,
        "1985150602287291935568054521177171638300868978215655730859378665066344726373823718423869104263333984641494340347905"_uint384);
  * g1.AddStatus(g2);
  * @endcode
  *
  */
  int AddStatus(const G2& other) {
    return ::bls12381_g2_add(
        this->x_[0].Values(), this->x_[1].Values(), this->y_[0].Values(),
        this->y_[1].Values(), other.x_[0].Values(), other.x_[1].Values(),
        other.y_[0].Values(), other.y_[1].Values(), this->x_[0].Values(),
        this->x_[1].Values(), this->y_[0].Values(), this->y_[1].Values());
  }

  /**
    * @brief to perform point addition on a crypto twist defined over quadratic
    extension of the base field
    *
    * @param other twist point on a bls12381 crypto
    * @return object reference of self
   */
  G2& Add(const G2& other) {
    //    AddStatus(other);
    platon_assert(AddStatus(other) == 0);
    return *this;
  }

  /**
  * @brief to perform point multiplication on a crypto twist defined over
  quadratic extension of the base field
  *
  * @param scalar scalar value
  * @return -1 argument error, 0 success
  *
  * Example:
  *
  * @code
  * G2
  g1("3059144344244213709971259814753781636986470325476647558659373206291635324768958432433509563104347017837885763365758"_uint384,
  *
  "352701069587466618187139116011060144890029952792775240219908644239793785735715026873347600343865175952761926303160"_uint384,
  *
  "927553665492332455747201965776037880757740193453592970025027978793976877002675564980949289727957565575433344219582"_uint384,
  *
  "1985150602287291935568054521177171638300868978215655730859378665066344726373823718423869104263333984641494340347905"_uint384);
  * G2
  g2("3059144344244213709971259814753781636986470325476647558659373206291635324768958432433509563104347017837885763365758"_uint384,
        "352701069587466618187139116011060144890029952792775240219908644239793785735715026873347600343865175952761926303160"_uint384,
        "927553665492332455747201965776037880757740193453592970025027978793976877002675564980949289727957565575433344219582"_uint384,
        "1985150602287291935568054521177171638300868978215655730859378665066344726373823718423869104263333984641494340347905"_uint384);
  * g1.ScalarMulStatus(g2);
  * @endcode
  *
  */
  int ScalarMulStatus(const std::uint256_t& scalar) {
    return ::bls12381_g2_mul(
        this->x_[0].Values(), this->x_[1].Values(), this->y_[0].Values(),
        this->y_[1].Values(), scalar.Values(), this->x_[0].Values(),
        this->x_[1].Values(), this->y_[0].Values(), this->y_[1].Values());
  }

  /**
    * @brief to perform point multiplication on a crypto twist defined over
    quadratic extension of the base field
    *
    * @param scalar scalar value
    * @return object reference of self
    */
  G2& ScalarMul(const std::uint256_t& scalar) {
    platon_assert(ScalarMulStatus(scalar) == 0);
    return *this;
  }

  /**
   * @brief o perform multiexponentiation in G2 (curve over quadratic extension
   * of the base prime field)
   *
   * @param g2 G2 point array
   * @param values scalar array
   * @return -1 argument error, 0 success
   */
  int MulExpStatus(const std::span<G2> g2,
                   const std::span<std::uint256_t> values) {
    size_t len = g2.size();
    size_t size = sizeof(uint8_t*) * len;
    const uint8_t** x1 = (const uint8_t**)malloc(size);
    const uint8_t** x2 = (const uint8_t**)malloc(size);
    const uint8_t** y1 = (const uint8_t**)malloc(size);
    const uint8_t** y2 = (const uint8_t**)malloc(size);
    const uint8_t** v = (const uint8_t**)malloc(size);

    for (size_t i = 0; i < len; i++) {
      x1[i] = g2[i].x_[0].Values();
      x2[i] = g2[i].x_[1].Values();
      y1[i] = g2[i].y_[0].Values();
      y2[i] = g2[i].y_[1].Values();
      v[i] = values[i].Values();
    }

    return ::bls12381_g2_mul_exp(x1, x2, y1, y2, v, len, this->x_[0].Values(),
                                 this->x_[1].Values(), this->y_[0].Values(),
                                 this->y_[1].Values());
  }

  /**
   * @brief o perform multiexponentiation in G2 (curve over quadratic extension
   * of the base prime field)
   *
   * @param g2 G2 point array
   * @param values scalar array
   * @return object reference of self
   */
  G2& MulExp(const std::span<G2> g2, const std::span<std::uint256_t> values) {
    platon_assert(MulExpStatus(g2, values) == 0);
    return *this;
  }

  /**
   * @brief maps extension field element into the G2 point
   *
   * @param c0 field element
   * @param c1 field element
   * @return -1 argument error, 0 success
   */
  int MapStatus(std::uint384_t& c0, std::uint384_t& c1) {
    return ::bls12381_map_g2(c0.Values(), c1.Values(), this->x_[0].Values(),
                             this->x_[1].Values(), this->y_[0].Values(),
                             this->y_[1].Values());
  }

  /**
   * @brief maps extension field element into the G2 point
   *
   * @param c0 field element
   * @param c1 field element
   * @return object reference of self
   */
  G2& Map(std::uint384_t& c0, std::uint384_t& c1) {
    platon_assert(MapStatus(c0, c1) == 0);
    return *this;
  }

  static G2 Base() {
    return G2(
        "3059144344244213709971259814753781636986470325476647558659373206291635324768958432433509563104347017837885763365758"_uint384,
        "352701069587466618187139116011060144890029952792775240219908644239793785735715026873347600343865175952761926303160"_uint384,
        "927553665492332455747201965776037880757740193453592970025027978793976877002675564980949289727957565575433344219582"_uint384,
        "1985150602287291935568054521177171638300868978215655730859378665066344726373823718423869104263333984641494340347905"_uint384);
  }

  static G2 ScalarBaseMul(const std::uint256_t& a) {
    G2 g = Base();
    g.ScalarMul(a);
    return g;
  }

  static G2 MulExpG2(const std::span<G2> g2,
                     const std::span<std::uint256_t> values) {
    G2 r;
    r.MulExp(g2, values);
    return r;
  }

  static G2 MapG2(std::uint384_t& c0, std::uint384_t& c1) {
    G2 r;
    r.Map(c0, c1);
    return r;
  }

  std::uint384_t& X1() { return x_[0]; }

  std::uint384_t& Y1() { return y_[0]; }

  std::uint384_t& X2() { return x_[1]; }

  std::uint384_t& Y2() { return y_[1]; }

  const std::uint384_t& X1() const { return x_[0]; }

  const std::uint384_t& Y1() const { return y_[0]; }

  const std::uint384_t& X2() const { return x_[1]; }

  const std::uint384_t& Y2() const { return y_[1]; }

  friend int pairing(const std::span<G1> g1, const std::span<G2> g2);

 private:
  std::uint384_t x_[2];
  std::uint384_t y_[2];
};

inline G1 P1() { return G1::Base(); }

inline G2 P2() { return G2::Base(); }

inline G1 Neg(const G1& p) {
  G1 p1 = p;
  return p1.Neg();
}

inline G1 Addition(const G1& p1, const G1& p2) {
  G1 res;
  return res.Add(p1, p2);
}

inline G2 Addition(const G2& p1, const G2& p2) {
  G2 r = p1;
  return r.Add(p2);
}

inline G1 ScalarMul(const G1& p, const std::uint256_t& s) {
  G1 r;
  return r.ScalarMul(p, s);
}

inline G2 ScalarMul(const G2& p, const std::uint256_t& s) {
  G2 r = p;
  return r.ScalarMul(s);
}

/**
 * @brief  to perform a pairing operations between a set of pairs of (G1, G2)
 *points
 * @param g1 list of G1
 * @param g2 list of G2
 * @return -2 argument error -1 failed 0 success
 **/
int pairing(const std::span<G1> g1, const std::span<G2> g2) {
  size_t len = g1.size();
  size_t size = sizeof(uint8_t*) * len;
  const uint8_t** x1 = (const uint8_t**)malloc(size);
  const uint8_t** y1 = (const uint8_t**)malloc(size);
  const uint8_t** x11 = (const uint8_t**)malloc(size);
  const uint8_t** y11 = (const uint8_t**)malloc(size);
  const uint8_t** x12 = (const uint8_t**)malloc(size);
  const uint8_t** y12 = (const uint8_t**)malloc(size);
  for (size_t i = 0; i < len; i++) {
    x1[i] = g1[i].x_.Values();
    y1[i] = g1[i].y_.Values();
    x11[i] = g2[i].x_[0].Values();
    y11[i] = g2[i].y_[0].Values();
    x12[i] = g2[i].x_[1].Values();
    y12[i] = g2[i].y_[1].Values();
  }

  return ::bls12381_pairing(x1, y1, x11, x12, y11, y12, len);
}

}  // namespace bls12381
}  // namespace crypto
}  // namespace platon