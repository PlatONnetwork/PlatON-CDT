#include "platon/crypto/bls12381/bls12381.hpp"

namespace platon {
namespace crypto {
namespace bls12381 {
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
  return bls12381::pairing(g1, g2) == 0;
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

  void UpdateWithU384(const std::uint384_t &value) {
    bytes32 old_state_0 = state_0;
    EncodeKeccak256(DST_0, old_state_0, state_1, value, state_0.data());
    EncodeKeccak256(DST_1, old_state_0, state_1, value, state_1.data());
  }

  void UpdateWithFr(const Fr &value) { UpdateWithU256(value); }

  void UpdateWithG1(const G1 &p) {
    UpdateWithU384(p.X());
    UpdateWithU384(p.Y());
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
      "0x3fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

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
        "23674694431658770659612952115660802947967373701506253797663184111817857449850"_uint256,
        {
            G1{ "78038516820041786328109065899853013207762916509669108230267013626605748989528216340778531481980896635242229678380"_uint384,
                "831153453577249970209013211639930958706281809640093659676724418112998613880179014159481061337260949645283981345993"_uint384 },
            G1{ "1521877335125236995766864553570576413629691441838143882327745400746339776349209600574245263623981887891653960165555"_uint384,
                "1148197392889766714024912178652040779798838187238931490977661797033791042814066443258301920178525084178947639149649"_uint384 },
            G1{ "72074889608044742241078034089676286003544266603762579436249989025992822950771541048103878325011787809920005859797"_uint384,
                "712886015401090648618851248777806723409872522999077883787829538947108675746537165846627821429435219056480285222220"_uint384 },
            G1{ "1521877335125236995766864553570576413629691441838143882327745400746339776349209600574245263623981887891653960165555"_uint384,
                "2854212162331900679392877647083863376758044632700076394354396339090240607676771421184385708950490579858946633410138"_uint384 },
            G1{ "849026777362996308868400979481883766709548438572051695872701311934495062699085330530490048545194196141903750155597"_uint384,
                "2019967212550071407674099478622232201012759645379842129681547313311975581359104902043467231123953482592424840117426"_uint384 },
            G1{ "0"_uint384,
                "0"_uint384 }
        },
        {
            G1{
                "0"_uint384,
                "0"_uint384 }
        },
        {
            G1{ "1468912584472858917934907247608731836000249692641876704614329920484513125271105255336396610003939923368598823325392"_uint384,
                "1131958101825725917887771977381489503975715272139146215073083896121930763198740686846590065101959803519042081026409"_uint384 },
            G1{ "2877072735894108956393938156477921115804235650279978097031422352745035894941035185837079341788418051675653444218552"_uint384,
                "3727664396386833306013039559210883507481333721174650936761748053004708464535139504518080961755747437646437618457050"_uint384 },
            G1{ "1091707776553837196745038959291253467815989786382616562236975956304200158530480530753092852956732594595211413754149"_uint384,
                "2629562799397609843040060206707374755373395577530428998388148849633007545076987098786613011819603431766092964380856"_uint384 },
            G1{ "3872145415406717541463682714918441838738709142226354954467060060659887833966414742739605027697417180878784467761436"_uint384,
                "1203581368978605082446468198169998284314000043648191345412035648298876679615562728145192154142110311315278488104214"_uint384 }
        },
        {
            "5"_uint256,
            "7"_uint256,
            "10"_uint256
        },

        // we only have access to value of the d(x) witness polynomial on the next
        // trace step, so we only need one element here and deal with it in other
        // places by having this in mind
        G2{ "1923715283628129517893105842615959830363498702497664745974990588996209331813843589632414824001620944269440080379892"_uint384, "3864191984789782450686817001566090904665039450692004928837849820635132435026335075508774077106811264644352182862916"_uint384, "2704787439401728244111755366005061190023540865275853777207075226153642246732504508015020139058220971726263463845510"_uint384, "691239210007468902241606746699705833082826881077174879693843559839090599427295424361844288788816835134015808615901"_uint384 }
    };




    return vk;
  }

  Proof DeserializeProof(const std::vector<std::uint256_t> &public_inputs,
                         const std::vector<std::uint384_t> &serialized_proof) {
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
      const std::vector<std::uint384_t> &serialized_proof) {
    VerificationKey vk = get_verification_key();
    platon_assert(vk.num_inputs == public_inputs.size());

    Proof proof = DeserializeProof(public_inputs, serialized_proof);

    bool valid = Verify(proof, vk);

    return valid;
  }
};

}  // namespace plonk
}  // namespace bls12381
}  // namespace crypto
}  // namespace platon








#include "unit_test.hpp"

TEST_CASE(plonk, test) {
std::vector<std::uint256_t> publics = {
    1};
std::vector<std::uint384_t> proof = {
    std::uint384_t("0x9e86cb15f0c18e9451b67e515549759c68449a6a3f3d2372295ba79b0f2df7f49bf5624867f7df78308259c92e60445"),
    std::uint384_t("0x10d807c054f43d12c00addf974ffa0d14957cd726c051e30e288e83a9cd8b7836c76645599e57d26e364e1cb134fd565"),
    std::uint384_t("0x14d59221ee965e77563d609abecad156fef4e6c6891cf013b77bd6b4caf2748ff5310b9dd748ad4ec0e96e9202b71b43"),
    std::uint384_t("0xf6028115ad626af7b03a8d0413661361887e5bfd8b52e191361b6d1946603b179ee484f015208cb31b8f6a707e06123"),
    std::uint384_t("0x9e34941e778184db949c5313842c0a5a1e4ae19dbbbaa7c592bdaaafd70cdf771a0ec37916687314cf94d6071e868b3"),
    std::uint384_t("0x775c1d6b72f2616d2ae43a8034a0423fb545bb2c0dc2ea956b7f2dbb39e0167780b4986534137598860f56b82ed0451"),
    std::uint384_t("0x0"),
    std::uint384_t("0x0"),
    std::uint384_t("0x7cef474adbf37ce6de9e977236f30931160bd6a6ecc832f37c9bfc7f17fe4193572ce75272351f836010930636dddb3"),
    std::uint384_t("0x8a0a538fbc31e4c98b0a93f22a52238c0b7977dee3007dee7998abc05cd1754d6005b4eddf0d59b31e555ff25fc4b54"),
    std::uint384_t("0xffc3c98f26c9b923afc9b64610cc03f23649f995f6e3afbf935fb94f70b78cdc1fd5163200f3ce841ae2863457ced35"),
    std::uint384_t("0x1360f3110acad1b3f48c36027e1f2c1dfa0a41b0c1515d57703f6f61cc84224839fe17037adf224e6daefb688ce01c58"),
    std::uint384_t("0x19e8072b57c73fb6b8651c8d59a3b95c633ebf08a479a8ec6aeac2cad63fb81bff923dfc0850873a68be50c2bbd2d575"),
    std::uint384_t("0x1e70479dc27b431b01dc18ad21aab85f59e6472cef67d5e2daaa21288bf72bf290a24c6120806e19409ce3146bc47c3"),
    std::uint384_t("0x6e98d32afcc221e5cd8dfd32669ca26390f7829437a8797a109acbfeb34562907cad95828a4e4cf69e58ddf969d0b25"),
    std::uint384_t("0xada27cc8c718bc321a708fbada8b2f237bb393494e70993ea2a52c62cf1acb5e4860fc0ec67ac66b2f7598813cd4c82"),
    std::uint384_t("0x1406a2263e8fa50233c13ee3d995f962a4bb5a8e2f0a68cf2f1482fb8b772a88fbb7e8219449042a3ffbc3d5206c7e13"),
    std::uint384_t("0x8af017aefa2625465c1a678d8daf7bbe9df9ed052983cfaff72e4628bd3c47199a0c884bfe5bcb67eddd4f4c2e5295"),
    std::uint384_t("0x6076c56d5eabe685bce43062381a3316a4c2a7752a2eff00382f8c31d3e601a0"),
    std::uint384_t("0x51b3c38bef881533fedfad411caea2748892f66950ca2a8e277da2fcb8ea7de9"),
    std::uint384_t("0x42f11e90a0de0eb76e0f9c21e89bb24a0c48d0a0094d346b42ca78e902517631"),
    std::uint384_t("0x0"),
    std::uint384_t("0x0"),
    std::uint384_t("0x6f8d01db80a1b7ec880ab97f496ee4974aed145f32588c69091a8efd79a48ee2"),
    std::uint384_t("0x128e666597d97506650b36affc5485c70bda9f3dbe922e01ab1ae3d9ea8cec7"),
    std::uint384_t("0x2afdfc9f102835634f5b6f08d097cd9a7ecf072780815666966114b3b5abc94e"),
    std::uint384_t("0x25e3a6296412001c5086431f76ea097467a059c6c3ef4d627b14156a18881de6"),
    std::uint384_t("0x64c6e09d58c98527fc145b4a57ebba22a9adead1b3380c9b3c4039171eea9c6a"),
    std::uint384_t("0x2e1cbd033414911490613b6d4289ba4461538296fa4a89583dac13d3d0fc8367"),
    std::uint384_t("0x18d225fdf2de3dc6c569280125c25b1835eab58bd5b2db0a094db774c58cb5acf29642d1ec80dc0e3fb946d7641d1b6d"),
    std::uint384_t("0x98969fc234858042d65b7e3d641a439b67b487bd7df7281a9a39aea49fabd8cd7319daca06792a083e8c22611187beb"),
    std::uint384_t("0x4474091ec91bf7b0495c80de734562993a1936087052f3e10d309c31c4ba2ccc4e2cd949e58fe87149935b602799f7b"),
    std::uint384_t("0xf27b5a75ed9687a4433b1ee0edcf2db83bde07db62cd1fda066d0e3b6c3391e8815a067b2f5d18b80a891f713139f4")
    };
platon::crypto::bls12381::plonk::KeyedVerifier verifier;
println("result:", verifier.VerifySerializedProof(publics, proof));
}
UNITTEST_MAIN() { RUN_TEST(plonk, test) }
