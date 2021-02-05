#undef NDEBUG
#include "platon/platon.hpp"
#include "unit_test.hpp"

using namespace platon;


#include <array>
#include "platon/platon.hpp"
using namespace platon;

const std::uint256_t PRIME_Q = "21888242871839275222246405745257275088696311157297823662689037894645226208583"_uint256;
const std::uint256_t SNARK_SCALAR_FIELD = "21888242871839275222246405745257275088548364400416034343698204186575808495617"_uint256;

class Verify : public platon::Contract{
private:
	struct G1Point{
		std::uint256_t X;
		std::uint256_t Y;
	};

	// Encoding of field elements is: X[0] * z + X[1]
	struct G2Point{
		std::array<std::uint256_t, 2> X;
		std::array<std::uint256_t, 2> Y;
	};

private:
	// return The negation of p, i.e. p.plus(p.negate()) should be zero.
	G1Point negate(const G1Point &p){
		if (p.X == 0 && p.Y == 0)
		{
			return G1Point{0, 0};
		}

		return G1Point{p.X, PRIME_Q - (p.Y % PRIME_Q)};
	}

	// return The sum of two points of G1
	G1Point plus(const G1Point &p1, const G1Point &p2){
		G1Point temp;
		bn256_g1_add(p1.X.Values(), p1.Y.Values(), p2.X.Values(), p2.Y.Values(), temp.X.Values(), temp.Y.Values());
		return temp;
	}

	/*
   * @return The product of a point on G1 and a scalar, i.e.
   * p == p.scalar_mul(1) and p.plus(p) == p.scalar_mul(2) for all
   * points p.
   */
	G1Point scalar_mul(const G1Point &p1, const std::uint256_t &s){
		G1Point temp;
		bn256_g1_mul(p1.X.Values(), p1.Y.Values(), s.Values(), temp.X.Values(), temp.Y.Values());
		return temp;
	}

	/* @return The result of computing the pairing check
   * e(p1[0], p2[0]) *  .... * e(p1[n], p2[n]) == 1
   * For example,
   * pairing([P1(), P1().negate()], [P2(), P2()]) should return true.
   */
	bool pairing(const G1Point &a1, const G2Point &a2, const G1Point &b1,
				 const G2Point &b2, const G1Point &c1, const G2Point &c2,
				 const G1Point &d1, const G2Point &d2){
		G1Point p1[4] = {a1, b1, c1, d1};
		G2Point p2[4] = {a2, b2, c2, d2};

		size_t len = sizeof(p1) / sizeof(p1[0]);
		size_t size = sizeof(uint8_t *) * len;
		uint8_t **x1 = (uint8_t **)malloc(size);
		uint8_t **y1 = (uint8_t **)malloc(size);
		uint8_t **x11 = (uint8_t **)malloc(size);
		uint8_t **y11 = (uint8_t **)malloc(size);
		uint8_t **x12 = (uint8_t **)malloc(size);
		uint8_t **y12 = (uint8_t **)malloc(size);

		for (size_t i = 0; i < len; i++){
			x1[i] = p1[i].X.Values();
			y1[i] = p1[i].Y.Values();
			x11[i] = p2[i].X[0].Values();
			y11[i] = p2[i].Y[0].Values();
			x12[i] = p2[i].X[1].Values();
			y12[i] = p2[i].Y[1].Values();
		}

		return 0 == bn256_pairing(x1, y1, x11, x12, y11, y12, len);
	}

private:
	struct VerifyingKey{
		G1Point alfa1;
		G2Point beta2;
		G2Point gamma2;
		G2Point delta2;
		std::array<G1Point, 2> IC;
	};

	struct Proof{
		G1Point A;
		G2Point B;
		G1Point C;
	};

	// 获取 vk 的值
	VerifyingKey get_verifyingKey(){
		VerifyingKey vk;

	    vk.alfa1 = G1Point{"12025617081811644466540568594203627915428876491275539831394509524350972889293"_uint256, "5762527821561390169564717038624613525313261488627764775042655308469844907423"_uint256};
		vk.beta2 = G2Point{ {"17469251224830624740963083079068143512035617829645821640193547375816234376183"_uint256, "11525047700836521778424717458670467335931164532016274526532009685539729707271"_uint256}, {"1878872153025504724784589694957109804403566649852557285513542344286567641518"_uint256, "16854978121148497063442767126901649128570209116286604202468998893486406784700"_uint256} };
		vk.gamma2 = G2Point{ {"19137019593668175202496932966890510920485673356726379389830467981018445866367"_uint256, "20972161800482909290912687010749869363868085374677974400679892451983042758383"_uint256}, {"12671264313535825087798401513228501040957187471897983161607550317813506167972"_uint256, "2846121156602612453315895899690886428797970018761829971659630546700414909276"_uint256} };
		vk.delta2 = G2Point{ {"3382100435057716034408801010861825419963317535950235733073839884472453004221"_uint256, "11386786782380796698008238411941385747174753265414630943931922507471352791007"_uint256}, {"8612260367432331815273540810438994416424674177258297864668239626453184682"_uint256, "19487083420319371248407595647518048833508541635882062671957885340131922363901"_uint256} };   
		vk.IC[0] = G1Point{ "16180604439674321509512531459789894282456872108346186120182080268922604276395"_uint256, "20797476474257098354983862596860376722058164763887947382608482933225826166335"_uint256};   
		vk.IC[1] = G1Point{ "2781780121661912873644617287969078147266188039959889120926784156258791877145"_uint256, "9494489029035150821775035208420040260241865583091108348298079625003908882355"_uint256};

		return vk;
	}

public:
	CONST void init() {}

	using Array2Uint256 = std::array<std::uint256_t, 2>;
	CONST bool verify_proof(Array2Uint256 a, std::array<Array2Uint256, 2> b, Array2Uint256 c, std::array<std::uint256_t, 1> input){
		Proof proof{G1Point{a[0], a[1]}, G2Point{ {b[0][0], b[0][1]}, {b[1][0], b[1][1]} }, G1Point{c[0], c[1]}};
		VerifyingKey vk = get_verifyingKey();

		// Compute the linear combination vk_x
		G1Point vk_x = G1Point{0, 0};

		// Make sure that proof.A, B, and C are each less than the prime q
		if (proof.A.X > PRIME_Q) platon_revert();
		if (proof.A.Y > PRIME_Q) platon_revert();
		if (proof.B.X[0] > PRIME_Q) platon_revert();
		if (proof.B.Y[0] > PRIME_Q) platon_revert();
		if (proof.B.X[1] > PRIME_Q) platon_revert();
		if (proof.B.Y[1] > PRIME_Q) platon_revert();
		if (proof.C.X > PRIME_Q) platon_revert();
		if (proof.C.Y > PRIME_Q) platon_revert();

		// Make sure that every input is less than the snark scalar field
		int length = input.size();
		for (int i = 0; i < length; i++){
			if (input[i] > SNARK_SCALAR_FIELD)
				platon_revert();
			vk_x = plus(vk_x, scalar_mul(vk.IC[i + 1], input[i]));
		}
		vk_x = plus(vk_x, vk.IC[0]);

		// result
		return pairing(negate(proof.A), proof.B, vk.alfa1, vk.beta2, vk_x,
					   vk.gamma2, proof.C, vk.delta2);
	}
};



TEST_CASE(gnark, zk) {
    Verify ver;
    Verify::Array2Uint256 a;
    a[0] = "5482574794458660612675928363544872510673657974993808207263066874294944300629"_uint256;
    a[1] = "170467064439728718934352671133327699323363533285598835363760075450070398800"_uint256;

    std::array<Verify::Array2Uint256, 2> b;
    b[0][0] = "1208709018463180984969854432057104813396199805829702687898359853500773619001"_uint256;
    b[0][1] = "6696838729553247875574310350012233538971672417096296810372965362165676895279"_uint256;
    b[1][0] = "10135583719019820512475109572501322385601902625561030886675638618637214319298"_uint256;
    b[1][1] = "7526294757328958406518019427068243740665448416142217470668353922848775638141"_uint256;


    Verify::Array2Uint256 c;
    c[0] = "18412364119336227288423068810388651140834462203657980269815371996001163531042"_uint256;
    c[1] = "21256003553136597216287063790710823513408735845767604344907220609024316585492"_uint256;

    std::array<std::uint256_t, 1> input;
   input[0] = "35"_uint256;

    bool result = ver.verify_proof(a, b, c, input);
    ASSERT(result);
	input[0] = "356"_uint256;
	result = ver.verify_proof(a, b, c, input);
	ASSERT(!result);
}

UNITTEST_MAIN() {
  RUN_TEST(gnark, zk);
  }