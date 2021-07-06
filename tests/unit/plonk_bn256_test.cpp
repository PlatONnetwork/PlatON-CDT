#include "unit_test.hpp"



#include "platon/crypto/bn256/bn256.hpp"

namespace platon {
namespace crypto {
namespace bn256 {
namespace plonk {
typedef std::array<uint8_t, 32> bytes32;
typedef std::array<uint8_t, 4> bytes4;
typedef std::uint256_t Fr;

Fr ExpMod(const Fr &base, const Fr &exp) { return Exp(base, exp, Order); }

Fr Inverse(const Fr &fr) { return ExpMod(fr, Order - 2); }

void AddAssign(Fr &self, const Fr &other) { self = AddMod(self, other, Order); }

void SubAssign(Fr &self, const Fr &other) {
  self = AddMod(self, Order - other, Order);
}

void MulAssign(Fr &self, const Fr &other) { self = MulMod(self, other, Order); }
bool PairingProd2(const G1 &a1, const G2 &a2, const G1 &b1, const G2 &b2) {
  std::array<G1, 2> g1{a1, b1};
  std::array<G2, 2> g2{a2, b2};
  return bn256::pairing(g1, g2) == 0;
}
struct Transcript {
 public:
  Transcript() {
    state_0 = {0};
    state_1 = {0};
    challenge_counter = 0;
  }

  void UpdateWithU256(const std::uint256_t &value) {
    bytes32 old_state_0 = state_0;
    EncodeKeccak256(DST_0, old_state_0, state_1, value, state_0.data());
    EncodeKeccak256(DST_1, old_state_0, state_1, value, state_1.data());
  }

  void UpdateWithFr(const Fr &value) { UpdateWithU256(value); }

  void UpdateWithG1(const G1 &p) {
    UpdateWithU256(p.X());
    UpdateWithU256(p.Y());
  }

  Fr GetChallenge() {
    Fr query;
    EncodeKeccak256(DST_CHALLENGE, state_0, state_1, challenge_counter,
                    query.Values());
    challenge_counter += 1;
    Fr challenge = query & FR_MASK;
    return challenge;
  }

 private:
  template <size_t N>
  void EncodeKeccak256(const bytes4 &dst, const bytes32 &s0, const bytes32 &s1,
                       const std::WideUint<N> &value, uint8_t *res) {
    std::array<uint8_t, 68 + N/8> input;
    copy(input.data(), dst.data(), dst.size());
    copy(input.data() + 4, s0.data(), s0.size());
    copy(input.data() + 36, s1.data(), s1.size());
    copy(input.data() + 68, value.Values(), value.arr_size);
    platon_sha3(input.data(), input.size(), res, 32);
  }

  void copy(uint8_t *dest, const uint8_t* src, size_t size){
    uint8_t zero = 0;
    bigint_binary_operator(src, false, size, &zero, \
                         false, 1, dest,  \
                         size, ADD);
  }
  // flip 0xe000000000000000000000000000000000000000000000000000000000000000;
  static constexpr std::uint256_t FR_MASK = std::uint256_t(
      "0x1fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

  static constexpr bytes4 DST_0 = {0, 0, 0, 0};
  static constexpr bytes4 DST_1 = {0, 0, 0, 1};
  static constexpr bytes4 DST_CHALLENGE = {0, 0, 0, 2};

  bytes32 state_0;
  bytes32 state_1;
  std::WideUint<32> challenge_counter;
};

struct Plonk4VerifierWithAccessToDNext {
  static constexpr size_t STATE_WIDTH = 4;
  static constexpr size_t ACCESSIBLE_STATE_POLYS_ON_NEXT_STEP = 1;

  struct VerificationKey {
    std::uint256_t domain_size;
    size_t num_inputs;
    Fr omega;
    std::array<G1, STATE_WIDTH + 2> selector_commitments;  // STATE_WIDTH for witness +
    // multiplication + constant
    std::array<G1, ACCESSIBLE_STATE_POLYS_ON_NEXT_STEP> next_step_selector_commitments;
    std::array<G1, STATE_WIDTH> permutation_commitments;
    std::array<Fr, STATE_WIDTH - 1> permutation_non_residues;
    G2 g2_x;
  };

  struct Proof {
    std::vector<std::uint256_t> input_values;
    std::array<G1, STATE_WIDTH> wire_commitments;
    G1 grand_product_commitment;
    std::array<G1, STATE_WIDTH> quotient_poly_commitments;
    std::array<Fr, STATE_WIDTH> wire_values_at_z;
    std::array<Fr, ACCESSIBLE_STATE_POLYS_ON_NEXT_STEP> wire_values_at_z_omega;
    Fr grand_product_at_z_omega;
    Fr quotient_polynomial_at_z;
    Fr linearization_polynomial_at_z;
    std::array<Fr, STATE_WIDTH - 1> permutation_polynomials_at_z;

    G1 opening_at_z_proof;
    G1 opening_at_z_omega_proof;
  };

  struct PartialVerifierState {
    Fr alpha;
    Fr beta;
    Fr gamma;
    Fr v;
    Fr u;
    Fr z;
    std::vector<Fr> cached_lagrange_evals;
  };

  Fr EvaluateLagrangePolyOutOfDomain(const std::uint256_t &poly_num,
                                     const std::uint256_t &domain_size,
                                     const Fr &omega, const Fr &at) {
    platon_assert(poly_num < domain_size);
    Fr res;
    Fr one = 1;
    Fr omega_power = ExpMod(omega, poly_num);
    res = ExpMod(at, domain_size);
    SubAssign(res, one);
    platon_assert(res != 0);  // Vanishing polynomial can not be zero at point `at`
    MulAssign(res, omega_power);

    Fr den = at;
    SubAssign(den, omega_power);
    MulAssign(den, domain_size);

    den = Inverse(den);

    MulAssign(res, den);
    return res;
  }

  std::vector<Fr> BatchEvaluateLagrangePolyOutOfDomain(
      const std::vector<std::uint256_t> &poly_nums,
      const std::uint256_t &domain_size, const Fr &omega, const Fr &at) {
    std::vector<Fr> res;
    Fr one = 1;
    Fr tmp_1 = 0;
    Fr tmp_2 = domain_size;
    Fr vanishing_at_z = ExpMod(at, domain_size);
    SubAssign(vanishing_at_z, one);
    // we can not have random point z be in domain
    platon_assert(vanishing_at_z != 0);
    std::vector<Fr> nums;
    nums.resize(poly_nums.size());
    std::vector<Fr> dens;
    dens.resize(poly_nums.size());
    // numerators in a form omega^i * (z^n - 1)
    // denoms in a form (z - omega^i) * N
    for (size_t i = 0; i < poly_nums.size(); i++) {
      tmp_1 = ExpMod(omega, poly_nums[i]);  // power of omega
      nums[i] = vanishing_at_z;
      MulAssign(nums[i], tmp_1);

      dens[i] = at;  // (X - omega^i) * N
      SubAssign(dens[i], tmp_1);
      MulAssign(dens[i], tmp_2);  // mul by domain size
    }

    std::vector<Fr> partial_products;
    partial_products.resize(poly_nums.size());
    partial_products[0] = 1;
    for (size_t i = 1; i < dens.size(); i++) {
      partial_products[i] = dens[i - 1];
      MulAssign(partial_products[i], partial_products[i - 1]);
    }

    tmp_2 = partial_products[partial_products.size() - 1];
    MulAssign(tmp_2, dens[dens.size() - 1]);
    tmp_2 = Inverse(tmp_2);  // tmp_2 contains a^-1 * b^-1 (with! the last one)


    for (int i = dens.size() - 1; i >= 0; i--) {


      tmp_1 = tmp_2;                          // all inversed
      MulAssign(tmp_1, partial_products[i]);  // clear lowest terms
      MulAssign(tmp_2, dens[i]);
      dens[i] = tmp_1;
    }

    for (size_t i = 0; i < nums.size(); i++) {
      MulAssign(nums[i], dens[i]);
    }

    return nums;
  }

  Fr EvaluateVanishing(const std::uint256_t &domain_size, const Fr &at) {
    Fr res = ExpMod(at, domain_size);
    SubAssign(res, 1);
    return res;
  }

  bool VerifyAtZ(const PartialVerifierState &state, const Proof &proof,
                 const VerificationKey &vk) {
    Fr lhs = EvaluateVanishing(vk.domain_size, state.z);
    platon_assert(lhs != 0);  // we can not check a polynomial relationship if point
    // `z` is in the domain
    MulAssign(lhs, proof.quotient_polynomial_at_z);

    Fr quotient_challenge = 1;
    Fr rhs = proof.linearization_polynomial_at_z;

    // public inputs
    Fr tmp = 0;
    for (size_t i = 0; i < proof.input_values.size(); i++) {
      tmp = state.cached_lagrange_evals[i];
      MulAssign(tmp, proof.input_values[i]);
      AddAssign(rhs, tmp);
    }

    MulAssign(quotient_challenge, state.alpha);

    Fr z_part = proof.grand_product_at_z_omega;
    for (size_t i = 0; i < proof.permutation_polynomials_at_z.size(); i++) {
      tmp = proof.permutation_polynomials_at_z[i];
      MulAssign(tmp, state.beta);
      AddAssign(tmp, state.gamma);
      AddAssign(tmp, proof.wire_values_at_z[i]);
      MulAssign(z_part, tmp);
    }

    tmp = state.gamma;
    // we need a wire value of the last polynomial in enumeration
    AddAssign(tmp, proof.wire_values_at_z[STATE_WIDTH - 1]);

    MulAssign(z_part, tmp);
    MulAssign(z_part, quotient_challenge);

    SubAssign(rhs, z_part);

    MulAssign(quotient_challenge, state.alpha);

    tmp = state.cached_lagrange_evals[0];
    MulAssign(tmp, quotient_challenge);

    SubAssign(rhs, tmp);

    return lhs == rhs;
  }

  G1 ReconstructD(const PartialVerifierState &state, const Proof &proof,
                  const VerificationKey &vk) {
    // we compute what power of v is used as a delinearization factor in batch
    // opening of commitments. Let's label W(x) = 1 / (x - z) *
    // [
    // t_0(x) + z^n * t_1(x) + z^2n * t_2(x) + z^3n * t_3(x) - t(z)
    // + v (r(x) - r(z))
    // + v^{2..5} * (witness(x) - witness(z))
    // + v^(6..8) * (permutation(x) - permutation(z))
    // ]
    // W'(x) = 1 / (x - z*omega) *
    // [
    // + v^9 (z(x) - z(z*omega)) <- we need this power
    // + v^10 * (d(x) - d(z*omega))
    // ]
    //
    // we pay a little for a few arithmetic operations to not introduce another
    // constant
    std::uint256_t power_for_z_omega_opening = 1 + 1 + STATE_WIDTH + STATE_WIDTH - 1;
    G1 res = vk.selector_commitments[STATE_WIDTH + 1];

    G1 tmp_g1 = G1::Base();
    Fr tmp_fr = 0;

    // addition gates
    for (size_t i = 0; i < STATE_WIDTH; i++) {
      tmp_g1 = ScalarMul(vk.selector_commitments[i], proof.wire_values_at_z[i]);
      res.Add(tmp_g1);
    }

    // multiplication gate
    tmp_fr = proof.wire_values_at_z[0];
    MulAssign(tmp_fr, proof.wire_values_at_z[1]);
    tmp_g1 = ScalarMul(vk.selector_commitments[STATE_WIDTH], tmp_fr);
    res.Add(tmp_g1);

    // d_next
    tmp_g1 = ScalarMul(vk.next_step_selector_commitments[0],
                       proof.wire_values_at_z_omega[0]);
    res.Add(tmp_g1);

    // z * non_res * beta + gamma + a
    Fr grand_product_part_at_z = state.z;
    MulAssign(grand_product_part_at_z, state.beta);
    AddAssign(grand_product_part_at_z, proof.wire_values_at_z[0]);
    AddAssign(grand_product_part_at_z, state.gamma);
    for (size_t i = 0; i < vk.permutation_non_residues.size(); i++) {
      tmp_fr = state.z;
      MulAssign(tmp_fr, vk.permutation_non_residues[i]);
      MulAssign(tmp_fr, state.beta);
      AddAssign(tmp_fr, state.gamma);
      AddAssign(tmp_fr, proof.wire_values_at_z[i + 1]);

      MulAssign(grand_product_part_at_z, tmp_fr);
    }

    MulAssign(grand_product_part_at_z, state.alpha);

    tmp_fr = state.cached_lagrange_evals[0];
    MulAssign(tmp_fr, state.alpha);
    MulAssign(tmp_fr, state.alpha);

    AddAssign(grand_product_part_at_z, tmp_fr);

    Fr grand_product_part_at_z_omega =
        ExpMod(state.v, power_for_z_omega_opening);
    MulAssign(grand_product_part_at_z_omega, state.u);

    Fr last_permutation_part_at_z = 1;
    for (size_t i = 0; i < proof.permutation_polynomials_at_z.size(); i++) {
      tmp_fr = state.beta;
      MulAssign(tmp_fr, proof.permutation_polynomials_at_z[i]);
      AddAssign(tmp_fr, state.gamma);
      AddAssign(tmp_fr, proof.wire_values_at_z[i]);

      MulAssign(last_permutation_part_at_z, tmp_fr);
    }

    MulAssign(last_permutation_part_at_z, state.beta);
    MulAssign(last_permutation_part_at_z, proof.grand_product_at_z_omega);
    MulAssign(last_permutation_part_at_z, state.alpha);

    // add to the linearization
    tmp_g1 = ScalarMul(proof.grand_product_commitment, grand_product_part_at_z);
    tmp_g1.Add(Neg(ScalarMul(vk.permutation_commitments[STATE_WIDTH - 1],
                             last_permutation_part_at_z)));

    res.Add(tmp_g1);
    res.ScalarMul(state.v);

    res.Add(ScalarMul(proof.grand_product_commitment,
                      grand_product_part_at_z_omega));
    return res;
  }

  bool VerifyCommitments(const PartialVerifierState &state, const Proof &proof,
                         const VerificationKey &vk) {
    G1 d = ReconstructD(state, proof, vk);

    Fr z_in_domain_size = ExpMod(state.z, vk.domain_size);

    G1 tmp_g1 = G1::Base();

    Fr aggregation_challenge = 1;

    G1 commitment_aggregation = proof.quotient_poly_commitments[0];
    Fr tmp_fr = 1;
    for (size_t i = 1; i < proof.quotient_poly_commitments.size(); i++) {
      MulAssign(tmp_fr, z_in_domain_size);
      tmp_g1 = ScalarMul(proof.quotient_poly_commitments[i], tmp_fr);
      commitment_aggregation.Add(tmp_g1);
    }

    MulAssign(aggregation_challenge, state.v);
    commitment_aggregation.Add(d);

    for (size_t i = 0; i < proof.wire_commitments.size(); i++) {
      MulAssign(aggregation_challenge, state.v);
      tmp_g1 = ScalarMul(proof.wire_commitments[i], aggregation_challenge);
      commitment_aggregation.Add(tmp_g1);
    }

    for (size_t i = 0; i < vk.permutation_commitments.size() - 1; i++) {
      MulAssign(aggregation_challenge, state.v);
      tmp_g1 = ScalarMul(vk.permutation_commitments[i], aggregation_challenge);
      commitment_aggregation.Add(tmp_g1);
    }

    MulAssign(aggregation_challenge, state.v);

    MulAssign(aggregation_challenge, state.v);

    tmp_fr = aggregation_challenge;
    MulAssign(tmp_fr, state.u);
    tmp_g1 = ScalarMul(proof.wire_commitments[STATE_WIDTH - 1], tmp_fr);
    commitment_aggregation.Add(tmp_g1);

    // collect opening values
    aggregation_challenge = 1;

    Fr aggregated_value = proof.quotient_polynomial_at_z;

    MulAssign(aggregation_challenge, state.v);

    tmp_fr = proof.linearization_polynomial_at_z;
    MulAssign(tmp_fr, aggregation_challenge);
    AddAssign(aggregated_value, tmp_fr);

    for (size_t i = 0; i < proof.wire_values_at_z.size(); i++) {
      MulAssign(aggregation_challenge, state.v);

      tmp_fr = proof.wire_values_at_z[i];
      MulAssign(tmp_fr, aggregation_challenge);
      AddAssign(aggregated_value, tmp_fr);
    }

    for (size_t i = 0; i < proof.permutation_polynomials_at_z.size(); i++) {
      MulAssign(aggregation_challenge, state.v);

      tmp_fr = proof.permutation_polynomials_at_z[i];
      MulAssign(tmp_fr, aggregation_challenge);
      AddAssign(aggregated_value, tmp_fr);
    }

    MulAssign(aggregation_challenge, state.v);

    tmp_fr = proof.grand_product_at_z_omega;
    MulAssign(tmp_fr, aggregation_challenge);
    MulAssign(tmp_fr, state.u);
    AddAssign(aggregated_value, tmp_fr);

    MulAssign(aggregation_challenge, state.v);

    tmp_fr = proof.wire_values_at_z_omega[0];
    MulAssign(tmp_fr, aggregation_challenge);
    MulAssign(tmp_fr, state.u);
    AddAssign(aggregated_value, tmp_fr);

    commitment_aggregation.Add(Neg(G1::ScalarBaseMul(aggregated_value)));

    G1 pair_with_generator = commitment_aggregation;
    pair_with_generator.Add(ScalarMul(proof.opening_at_z_proof, state.z));

    tmp_fr = state.z;
    MulAssign(tmp_fr, vk.omega);
    MulAssign(tmp_fr, state.u);
    pair_with_generator.Add(ScalarMul(proof.opening_at_z_omega_proof, tmp_fr));

    G1 pair_with_x = ScalarMul(proof.opening_at_z_omega_proof, state.u);
    pair_with_x.Add(proof.opening_at_z_proof);
    pair_with_x.Neg();

    return PairingProd2(pair_with_generator, G2::Base(), pair_with_x, vk.g2_x);
  }

  bool VerifyInitial(PartialVerifierState &state, const Proof &proof,
                     const VerificationKey &vk) {
    platon_assert(proof.input_values.size() == vk.num_inputs);
    platon_assert(vk.num_inputs >= 1);
    Transcript transcript;
    for (size_t i = 0; i < vk.num_inputs; i++) {
      transcript.UpdateWithU256(proof.input_values[i]);
    }

    for (size_t i = 0; i < proof.wire_commitments.size(); i++) {
      transcript.UpdateWithG1(proof.wire_commitments[i]);
    }

    state.beta = transcript.GetChallenge();
    state.gamma = transcript.GetChallenge();

    transcript.UpdateWithG1(proof.grand_product_commitment);
    state.alpha = transcript.GetChallenge();

    for (size_t i = 0; i < proof.quotient_poly_commitments.size(); i++) {
      transcript.UpdateWithG1(proof.quotient_poly_commitments[i]);
    }

    state.z = transcript.GetChallenge();

    std::vector<std::uint256_t> lagrange_poly_numbers;
    lagrange_poly_numbers.resize(vk.num_inputs);
    for (size_t i = 0; i < lagrange_poly_numbers.size(); i++) {
      lagrange_poly_numbers[i] = i;
    }

    state.cached_lagrange_evals = BatchEvaluateLagrangePolyOutOfDomain(
        lagrange_poly_numbers, vk.domain_size, vk.omega, state.z);

    bool valid = VerifyAtZ(state, proof, vk);

    if (!valid) {
      return false;
    }

    for (size_t i = 0; i < proof.wire_values_at_z.size(); i++) {
      transcript.UpdateWithFr(proof.wire_values_at_z[i]);
    }

    for (size_t i = 0; i < proof.wire_values_at_z_omega.size(); i++) {
      transcript.UpdateWithFr(proof.wire_values_at_z_omega[i]);
    }

    for (size_t i = 0; i < proof.permutation_polynomials_at_z.size(); i++) {
      transcript.UpdateWithFr(proof.permutation_polynomials_at_z[i]);
    }

    transcript.UpdateWithFr(proof.quotient_polynomial_at_z);
    transcript.UpdateWithFr(proof.linearization_polynomial_at_z);
    transcript.UpdateWithFr(proof.grand_product_at_z_omega);

    state.v = transcript.GetChallenge();
    transcript.UpdateWithG1(proof.opening_at_z_proof);
    transcript.UpdateWithG1(proof.opening_at_z_omega_proof);
    state.u = transcript.GetChallenge();

    return true;
  }

  // This verifier is for a PLONK with a state width 4
  // and main gate equation
  // q_a(X) * a(X) +
  // q_b(X) * b(X) +
  // q_c(X) * c(X) +
  // q_d(X) * d(X) +
  // q_m(X) * a(X) * b(X) +
  // q_constants(X)+
  // q_d_next(X) * d(X*omega)
  // where q_{}(X) are selectors a, b, c, d - state (witness) polynomials
  // q_d_next(X) "peeks" into the next row of the trace, so it takes
  // the same d(X) polynomial, but shifted

  bool Verify(const Proof &proof, const VerificationKey &vk) {
    PartialVerifierState state;

    bool valid = VerifyInitial(state, proof, vk);

    if (!valid) {
      return false;
    }

    valid = VerifyCommitments(state, proof, vk);

    return valid;
  }
};

struct KeyedVerifier : public Plonk4VerifierWithAccessToDNext {
  static constexpr size_t SERIALIZED_PROOF_LENGTH = 33;

  VerificationKey get_verification_key() {
    VerificationKey vk = {
        8,
        1,
        "19540430494807482326159819597004422086093766032135589407132600596362845576832"_uint256,
        {
            G1{ "15433480046280319615008813672990690914626343026290595667286785235099036180946"_uint256,
                "8044854065228984566141525318176046303081763171922474813622576041966616090996"_uint256 },
            G1{ "11889302855860798267504618688753015326456153005311274745969457061110915966593"_uint256,
                "3120659310967718659748088263159161243044034806356339458739503554676457625938"_uint256 },
            G1{ "16268145788962265185820630686409891381084920518150310998208463249370090099274"_uint256,
                "17804462609003554104894606285290172234530736448835949236951211407205058320282"_uint256 },
            G1{ "11889302855860798267504618688753015326456153005311274745969457061110915966593"_uint256,
                "18767583560871556562498317482098113845652276350941484203949534339968768582645"_uint256 },
            G1{ "18438652286581150917550466821505214908170239515967139718578570204194906658288"_uint256,
                "6771273175534318478602786905310383352055710446424321928038805688238219483553"_uint256 },
            G1{ "0"_uint256,
                "0"_uint256 }
        },
        {
            G1{
                "0"_uint256,
                "0"_uint256 }
        },
        {
            G1{ "573043753990689242297518932500971014851136535512769031375089890701949664427"_uint256,
                "18878096269104978927252081503557663194085513869131753528691212378278784856126"_uint256 },
            G1{ "8961698090666250256809943281845265031579030796507698904533314025148425330609"_uint256,
                "9149936315150537347972210658699631912317340801926035195534208908432274831696"_uint256 },
            G1{ "20962122698668749637252193162792510655470254208774404369190828925836416826082"_uint256,
                "5271099758893795883822721655247339742996135395021447867712953823544565101930"_uint256 },
            G1{ "13873059811865727918435221184076967021750730023801440503223050982546925366538"_uint256,
                "11958456208993056691770858649553224464768700591966187467480337911288169900975"_uint256 }
        },
        {
            "5"_uint256,
            "7"_uint256,
            "10"_uint256
        },

        // we only have access to value of the d(x) witness polynomial on the next
        // trace step, so we only need one element here and deal with it in other
        // places by having this in mind
        G2{ "8346649071297262948544714173736482699128410021416543801035997871711276407441"_uint256, "7883069657575422103991939149663123175414599384626279795595310520790051448551"_uint256, "16795962876692295166012804782785252840345796645199573986777498170046508450267"_uint256, "3343323372806643151863786479815504460125163176086666838570580800830972412274"_uint256 }
    };




    return vk;
  }

  Proof DeserializeProof(const std::vector<std::uint256_t> &public_inputs,
                         const std::vector<std::uint256_t> &serialized_proof) {
    Proof proof;
    platon_assert(serialized_proof.size() == SERIALIZED_PROOF_LENGTH);
    proof.input_values.resize(public_inputs.size());
    for (size_t i = 0; i < public_inputs.size(); i++) {
      proof.input_values[i] = public_inputs[i];
    }

    size_t j = 0;
    for (size_t i = 0; i < STATE_WIDTH; i++) {
      proof.wire_commitments[i] = {serialized_proof[j],
                                   serialized_proof[j + 1]};

      j += 2;
    }

    proof.grand_product_commitment = {serialized_proof[j],
                                      serialized_proof[j + 1]};
    j += 2;

    for (size_t i = 0; i < STATE_WIDTH; i++) {
      proof.quotient_poly_commitments[i] = {serialized_proof[j],
                                            serialized_proof[j + 1]};

      j += 2;
    }

    for (size_t i = 0; i < STATE_WIDTH; i++) {
      proof.wire_values_at_z[i] = serialized_proof[j];

      j += 1;
    }

    for (size_t i = 0; i < proof.wire_values_at_z_omega.size(); i++) {
      proof.wire_values_at_z_omega[i] = serialized_proof[j];

      j += 1;
    }

    proof.grand_product_at_z_omega = serialized_proof[j];

    j += 1;

    proof.quotient_polynomial_at_z = serialized_proof[j];

    j += 1;

    proof.linearization_polynomial_at_z = serialized_proof[j];

    j += 1;

    for (size_t i = 0; i < proof.permutation_polynomials_at_z.size(); i++) {
      proof.permutation_polynomials_at_z[i] = serialized_proof[j];

      j += 1;
    }

    proof.opening_at_z_proof = {serialized_proof[j], serialized_proof[j + 1]};
    j += 2;

    proof.opening_at_z_omega_proof = {serialized_proof[j],
                                      serialized_proof[j + 1]};
    return proof;
  }

  bool VerifySerializedProof(
      const std::vector<std::uint256_t> &public_inputs,
      const std::vector<std::uint256_t> &serialized_proof) {
    VerificationKey vk = get_verification_key();
    platon_assert(vk.num_inputs == public_inputs.size());

    Proof proof = DeserializeProof(public_inputs, serialized_proof);

    bool valid = Verify(proof, vk);

    return valid;
  }
};

}  // namespace plonk
}  // namespace bn256
}  // namespace crypto
}  // namespace platon






TEST_CASE(plonk, test) {
  std::vector<std::uint256_t> publics = {
      std::uint256_t(
          1)};
  std::vector<std::uint256_t> proof = {
      std::uint256_t("0x22ef027a025c41211b53a0d125cd4451f9d691de851e0f1ed3043f3f062c4dc8"),
      std::uint256_t("0x1112234178b5406b9eb5f2e59ca74532900c6e491d75206e05cfab3ffafee041"),
      std::uint256_t("0x2c4951b832621f1b7315181f70905ba8cd4865753d98da5c28a474092edadbda"),
      std::uint256_t("0x27b14de154fd0af1394e5ca8f8c64032ee91cd9e046cbd7a24aeed18db9c3334"),
      std::uint256_t("0x1a491b60cdae811dc03c00655a64073cf536115324b340895c6db7e387eea681"),
      std::uint256_t("0x6e63b071abae5ad0dc281f103e234f34535d795699dcc6a895d5964422a5952"),
      std::uint256_t("0x0"),
      std::uint256_t("0x0"),
      std::uint256_t("0x30171216096075e15e11dbff790832749298b0b9307a81c14c0ad3d67170b410"),
      std::uint256_t("0x2a0ee64083bc91766bf8123d228afe19d4a209e7a44a35a350ec2b9f6c86aeec"),
      std::uint256_t("0x5eb55ed9f589c6d06d59dfa8b15ef5df8bc09527776bfce10bdc0da1f201196"),
      std::uint256_t("0xee17523cecf1ec406e97e62060f7bb49390e7cc17604bd035463bc8e372bba2"),
      std::uint256_t("0xb7e9c82177b89ee00471ced8ea6f649a915259ab447be8e4d8f5e38f3cd2764"),
      std::uint256_t("0x8de1337ca589a8f4092f75645d85bfd61ed8c3eca218c30eadc8e0f965964c2"),
      std::uint256_t("0xb291ef4be5652d0b9dce977dc2227df112877f23fe1eec07f5f1397b279e0f9"),
      std::uint256_t("0x128944dcd449b1003c8e496cc9c18fde46f2fc49e5d9b2e9f211998eccb8ef49"),
      std::uint256_t("0x2b407985c120d09312700dafda4750fa835b0831583072277c93e389ba6a3643"),
      std::uint256_t("0x24ba4668ca1ac25d9af3c795064bac7cf4d3f32554e06a300f5c44ee86b15d99"),
      std::uint256_t("0x16c390dc5dae009751e1f33a39bbaedeedef815d1ac351608dd6ee2874dc0514"),
      std::uint256_t("0x2041318a228de268737f461c3c711266cfb3bed4d379e8a64fbea71fb0e9a922"),
      std::uint256_t("0x21bffc38402217f4d87dcac82fe2dd59b0282a208b699a0c02cb5ec5dbdf0dec"),
      std::uint256_t("0x0"),
      std::uint256_t("0x0"),
      std::uint256_t("0x17652c20a5f168fc20cea26eebb87e4672253fdf6f625d70c319bd7bb9651d3d"),
      std::uint256_t("0x1d145d88e029c8452d8f1be0af69301257946d3379013ff10bc8e4417960ad31"),
      std::uint256_t("0xc01115d3fea2ef3416e9aae6f28abe76929c1f9a58b90f5909ee0c6afaf84cf"),
      std::uint256_t("0x441301e5e327afcf42d575f128fd2564b146559b27d888d446592ed3b17d0fb"),
      std::uint256_t("0x238a83fb51956a53b56b5725864f4b001a206168b47e452bcc9ee16d23f7595d"),
      std::uint256_t("0xc2bfedf0bdaed3f64b2e2dbe22f2f68dedd1b30bd677b4e71f65d4310741f75"),
      std::uint256_t("0x29502409ffe48759f4c617de245755799b0589d0471bf96b82ff07915ac7bce0"),
      std::uint256_t("0x20e7c6fd6a1d963b75eb181b3a6338703205d10f7f4254d4e1505596c263678c"),
      std::uint256_t("0x209c3f42b3cabce8888244206f6193d6686524709cadd457d6abbf7ecb64ce49"),
      std::uint256_t("0x2dcffc179f91960b15964c78c8d217d2f84dd73c2532f41d21afe00fd1a44d28")
      };
  platon::crypto::bn256::plonk::KeyedVerifier verifier;
  println("result:", verifier.VerifySerializedProof(publics, proof));
}

UNITTEST_MAIN() { RUN_TEST(plonk, test) }