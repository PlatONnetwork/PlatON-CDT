#pragma once

#include <array>
#include <vector>
#include "platon/bigint.hpp"

class Poseidon {
 public:
  using usize = unsigned int;

  static std::uint256_t Hash(const std::vector<std::uint256_t> &input) {
    usize t = input.size() + 1;
    constexpr usize n_rounds_f = 8;
    constexpr std::array<usize, 8> n_rounds_p_vec = {56, 57, 56, 60, 60, 63, 64, 63};
    usize n_rounds_p = n_rounds_p_vec[t - 2];

    std::vector<std::uint256_t> state(t);
    for (int i = 0; i < input.size(); i++) {
      state[i + 1] = input[i];
    }

    for (int i = 0; i < n_rounds_f + n_rounds_p; i++) {
      Ark(state, *(c_[t - 2]), i * t);
      Sbox(n_rounds_f, n_rounds_p, state, i);
      state = Mix(state, *(m_[t - 2]));
    }

    return state[0];
  }

 private:
  static constexpr std::uint256_t q = std::uint256_t(std::string_view("30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001"));
  static std::vector<std::vector<std::uint256_t>*> c_;
  static std::vector<std::vector<std::vector<std::uint256_t>>*> m_;

  static void Ark(std::vector<std::uint256_t> &state,
                  const std::vector<std::uint256_t> &para_c, usize it) {
    for (int i = 0; i < state.size(); i++) {
      std::uint512_t temp = state[i];
      state[i] = (temp + para_c[it + i]) % q;
    }
  }

  static void Sbox(usize n_rounds_f, usize n_rounds_p,
                   std::vector<std::uint256_t> &state, usize i) {
    if (i < n_rounds_f / 2 || i >= n_rounds_f / 2 + n_rounds_p) {
      for (int j = 0; j < state.size(); j++) {
        Exp5(state[j]);
      }
    } else {
      Exp5(state[0]);
    }
  }

  static std::vector<std::uint256_t> Mix(
      const std::vector<std::uint256_t> &state,
      const std::vector<std::vector<std::uint256_t>> &para_m) {
    std::uint256_t mul;
    std::vector<std::uint256_t> result;
    usize length = state.size();
    for (int i = 0; i < length; i++) {
      result.push_back(std::uint256_t());
      for (int j = 0; j < length; j++) {
        std::uint512_t temp = para_m[i][j];
        mul = (temp * state[j]) % q;

        std::uint256_t &newEl = result.back();
        newEl = (newEl + mul) % q;
      }
    }

    return result;
  }

  static void Exp5(std::uint256_t &a) {  a = Exp(a, std::uint256_t(5), q); }
};
