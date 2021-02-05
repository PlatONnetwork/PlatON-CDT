#pragma once
#include <array>
#include <vector>
#include <deque>
#include <cstring>
#include "platon/chain.hpp"
#include "platon/bigint.hpp"

class Mimc
{
private:
  static constexpr int rounds = 91;
  static constexpr std::string_view seed = "mimc";
  static constexpr std::uint256_t Q = std::uint256_t(std::string_view("30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001"));
  static constexpr std::array<std::uint256_t, 91> cts{
      std::uint256_t(),
      std::uint256_t(std::string_view("1799d93eb9d2848626dde66394f647da29ef8c47a2eb3be395fafe165a107a9e")),
      std::uint256_t(std::string_view("0898e040b547aa62baa333f0044d87f8e119af85d742766027ebae303c99bddc")),
      std::uint256_t(std::string_view("0497b9ca9032239274920a7f4e8ce6d86a515a8c9faeb3cd041776b4a1e2bbe6")),
      std::uint256_t(std::string_view("1f384c5c9ab758ce7cf97bde97e62d055e7edaac7c2121d227246fd789e3b1e7")),
      std::uint256_t(std::string_view("098640f821d635d35c5ed5e1121dfa1df9800e6a71b8a64f120feb9478580452")),
      std::uint256_t(std::string_view("1182c21d26c3bbda166729ba0fc09da97350619bc5b975824b69d0d51056e706")),
      std::uint256_t(std::string_view("2d5b816c1f7e2662d9b3621858ee487caef1289cdb5a56698cd8180f4e6abc48")),
      std::uint256_t(std::string_view("1401b926b8d7e064866670ed10bacbc608c70f11ea1a33b29b875e3faff5ad6d")),
      std::uint256_t(std::string_view("25fbbc4b6c6b7493c812f968a3bc677d2a7a2227043f82e18de3758d935c2e73")),
      std::uint256_t(std::string_view("1f04c6b3654f070ac4be06427f99fc41ca3becb44fc394be6ae7e5285b40e6cb")),
      std::uint256_t(std::string_view("042a8aab9589f7348dc1616c9604fa8357ed640eb9ada5f9cc348f031a3993ec")),
      std::uint256_t(std::string_view("28eabd13a7d96d73b73798d44dc55470b6bf6cd8560f644a535211950998db03")),
      std::uint256_t(std::string_view("02dd98473682c24cd3481ac3b2a93f77265743c7e6e26faf2cab9050080a6e15")),
      std::uint256_t(std::string_view("22c5451e70422dab98a2035bf824c70bf644f93157d7164c7069ff0834d76ce2")),
      std::uint256_t(std::string_view("2ab9711406a667721ffe37f53214ff8ecec7cc5fe8e9b767848e28734a4e36a1")),
      std::uint256_t(std::string_view("27cca53a553bef87e593b51858411f5fbb028279ad258a2962b90566855702ba")),
      std::uint256_t(std::string_view("2684cc7798b18d835e0b6704a9a8de2af0de493db92e181bfc08a07324a09999")),
      std::uint256_t(std::string_view("2935307f373490c9255ed57008cd8860be1a1c7d8111c9cab247af799f1169a9")),
      std::uint256_t(std::string_view("026fe389bcf40bdd6f1db8f37d414d701b4612b395ce2b1b5a2aceb5de72147d")),
      std::uint256_t(std::string_view("0cd72935d321dc8b7d2cd47d21b497b568710614a05b35141cb7852357fb5b24")),
      std::uint256_t(std::string_view("20a7703eb4805dfbc8d13dd9701b46c62a6d5dd37a9d7e107da123dae2452e2f")),
      std::uint256_t(std::string_view("218eb33983dd0949f4550fff773d8aa535b03c7cdea2fdc6607df78382adf697")),
      std::uint256_t(std::string_view("0d34cab9ed9092663b40c167bc28820c556a72778eec9929ca9d7efed89840a3")),
      std::uint256_t(std::string_view("137e786bcd86f11b5013ab11347c477dd0d06b98d694ff7da30f7e996f8b6680")),
      std::uint256_t(std::string_view("2754c7a06bd9bf66b5c172603a4e637b9918b68c8c9335b5f0ea94d58585e6e6")),
      std::uint256_t(std::string_view("16c9d5e9dc416d4b2dadf66b40237dc519d0ebad99ad5310109bbe4a87a92576")),
      std::uint256_t(std::string_view("1340308881ce8c0d09f786332030445326772efeb027e7b79d0a9e241f5e5327")),
      std::uint256_t(std::string_view("1458a55da3a3541da4a4f5d6c64a180e991416cd26dc2fea4a3fdda139509169")),
      std::uint256_t(std::string_view("1bdea8856759d01eb2bc234d7d70aa9978c49c781d0bb10ab9faab5a257f5332")),
      std::uint256_t(std::string_view("06652f203d73198ea3b3feab376f0a43eb3d50891c69159194a400d564f68845")),
      std::uint256_t(std::string_view("3026e28eef2814c703cba0cca07705e14b2f60c31125148689c87264567fb483")),
      std::uint256_t(std::string_view("1c180bc09db61af9cf9037e52e9e5484f385eecbf64ca745e0dfb20184a25aa5")),
      std::uint256_t(std::string_view("172c7342170eb31a0be8208b03168040d8e7577cb5e84dbdb7c65f3d422a5c41")),
      std::uint256_t(std::string_view("082dcf416745a746dac1ad7e7c3349066b64613caf2ae178bd9eea27581e1c19")),
      std::uint256_t(std::string_view("2c79e393526a045c78a0e632da22f63e22b91e766882fb6e2c29b54367d2f858")),
      std::uint256_t(std::string_view("17e06402d1feded3bc3d5db352d8e97715cd91b885092a4ffe479c589c633887")),
      std::uint256_t(std::string_view("20bc999e284a666669f82fa668d4dd3b66ba9af3ae954e5f6649b3d0b11e4ad3")),
      std::uint256_t(std::string_view("017d8dfc2eb2be87af3ef15ed04479fb4f51d5a8a87e68564495687da89c186f")),
      std::uint256_t(std::string_view("0d88f3c4554fb1a3afcf5eb65c37a4839c9a190301f2307d0ecf798072af49c9")),
      std::uint256_t(std::string_view("292f38249aec35b5a38971f04eeb08015dbc37194a49e60d304492a7e795b500")),
      std::uint256_t(std::string_view("06f119ad4f0ed501d876a69602a92f967560d8b053a3ca0ab1036bf4a13521da")),
      std::uint256_t(std::string_view("26103dd88a2e57e70f1bd7b459a0904e2267aba26bf9ba177708b7e69e54d3f4")),
      std::uint256_t(std::string_view("25d95187bca6ee58374b115d20134b2c20b109738346635f8970c4e8ab8bc959")),
      std::uint256_t(std::string_view("211778b94bb483d7afc8084e19ecd05a031c1f91d189e9cb2fa7ad8ba4ade834")),
      std::uint256_t(std::string_view("1c544da64ceaae737d0547fa294c9fb42a60a17f2ca4202d190da57334532e6b")),
      std::uint256_t(std::string_view("1e056f7a8b5a51cc38e71c408adc8dbf2c16d0a9bfabbdbffd37d49d19dc2f5c")),
      std::uint256_t(std::string_view("16f70ce286adcb34526c8977257619c6f15e09d4fcf07c291c509ddda6f9a65f")),
      std::uint256_t(std::string_view("2d5dc12a792f1e74ce61783e2db4a6a088c3eadbc2c463a06d2346011f94cff8")),
      std::uint256_t(std::string_view("22a276a6f19e533c7b5fc93ee100799b2624c926cbdc87c041242a7d96f9abe1")),
      std::uint256_t(std::string_view("1307be65554b2dc85d4f98c32499df55fa9a6a9eac55d851e6e674ce2fef41e5")),
      std::uint256_t(std::string_view("0a9a9b6027ca74b0e4075dadb45354f7407f03085b55a03f1a17412540ba0140")),
      std::uint256_t(std::string_view("2621b716894da6b4fdb0fe1d7930a4eff5163cac12f6afcaa56d113b823a50be")),
      std::uint256_t(std::string_view("290c65af7fd1a04f13b3e11c2bac749f22fc7ae4999f3f4490949c1fafef9c03")),
      std::uint256_t(std::string_view("0e63b7d7d1e5a2baed01191024a5d16a29300a58dab289b6ffc6e1a7f6bd79c8")),
      std::uint256_t(std::string_view("2cd3c7456173bb1b973d5a5dd1317888f50be1a3eee8915acedfdfdf99b06aa7")),
      std::uint256_t(std::string_view("25503cd0bd5f3cf6a75b377f05647f6ecc4f3f3c6f67d6b75238a266370bb8c8")),
      std::uint256_t(std::string_view("28a59c74da67f199ed89e72f86cb6a77a43f2dc79d69486029eca4c61f2252e8")),
      std::uint256_t(std::string_view("134e256da122612a8078b086349c6d1a5a5b8023755748e063e9e27d2c4c2d25")),
      std::uint256_t(std::string_view("1ed44e5e939a9a22f7495fc3cf22a9f60ad2f0834b3b20afcfd026adabe4cc83")),
      std::uint256_t(std::string_view("05ae000afd27edb7b0f1086bcccafd3016b7812dfb736502a70000b09a6d8f6a")),
      std::uint256_t(std::string_view("113fb39a0d2c38fa2688ec375b99dc8c9c3ff1857395d31951398cdd6c358435")),
      std::uint256_t(std::string_view("15898ad8aafef9789e86558780fdedba192d1e532bdeda695131e3feb5d70edf")),
      std::uint256_t(std::string_view("20c41bccc3e935d9512fd8c22bad188ac9da1034d371f1ed326e94067d5716fe")),
      std::uint256_t(std::string_view("1d90d1aa292874c6b5151d4d830c521df6bcc27318165311b59c9a8cff0b066a")),
      std::uint256_t(std::string_view("067d4d50e7107b37b80d4a84894401b3abcb699492f2bc3dac3b277342453f96")),
      std::uint256_t(std::string_view("15340c1119581ab82565f7d669bcb6025eac570a9b9ad06c5f84b3655ad5c495")),
      std::uint256_t(std::string_view("0b03e01318fb277afca7b4b702b31b400d698f353cd47d5c18ca331463d97e51")),
      std::uint256_t(std::string_view("11733fbb1a64471974fd4339ac85c1ca43c3a124f14916e9ae2709c31f7b96d9")),
      std::uint256_t(std::string_view("0db7ea15f8b9c8bb8f240611d23630712772ac0aac13808eec7352b2361cbd0c")),
      std::uint256_t(std::string_view("035e45eafd37f5e3b1de1a009821ef1f279895e299c2323e08899a60fbcba55f")),
      std::uint256_t(std::string_view("0df76295d8eae4d42834597489995ff9e37be9ab4101850ef222d549a6e441f0")),
      std::uint256_t(std::string_view("20c8645f40a266581cba2f1e41bfe6b54a5d74349cd91eb0e79315bef11c60da")),
      std::uint256_t(std::string_view("27d50906d806ce39d465150cff7bc57dcdfc46756aa38bbd7bbb5f9aa3854524")),
      std::uint256_t(std::string_view("1fb51e7b5f05518da4736b91598aefc8f903252e4f03e221775e29ff2e0cefd1")),
      std::uint256_t(std::string_view("14607a56dec5d32275b52c0ee489619c984812f0a1a846bae1ff6fd583bbdb7b")),
      std::uint256_t(std::string_view("163ff0efbdc1a419aa0f8bfc8cc52aede2dce83515b56398edd269938d3b2374")),
      std::uint256_t(std::string_view("070ee56aef4bca493ca57da2884d3a5ca62d352f6bd806b44f62026f35947112")),
      std::uint256_t(std::string_view("0e56cfe8d0eb9e334b7e565957e69f0f49947a2f3381532960d65548166514a4")),
      std::uint256_t(std::string_view("23870e6600c8e588761140e567be7da21f3d9928d1c094c21c1a826cf783d933")),
      std::uint256_t(std::string_view("2b21f45a70aeae9e15f3c6b09f1d5e4be8e98534cf393eb9447b85f6e817a17b")),
      std::uint256_t(std::string_view("215656eae2f012023240623f0930ac33bf44a3fea83a8e46dbfd956489e96fd7")),
      std::uint256_t(std::string_view("04a04c2d37b7e5362d82c2bf02d04e9715a8eed95bbd856b510c73536df21af0")),
      std::uint256_t(std::string_view("2d92f09b299458b5885df5bf5a83788b540bf888208ce017b1e11939c72839dc")),
      std::uint256_t(std::string_view("07d8ee809bcb95d766c1d6383cdd34c7b0d1fbcafc3602c5c073bcf8415c9b47")),
      std::uint256_t(std::string_view("069854d32af2a13862999cb0c3ddc36d62399fb47d26253e0692b858ed0aaa1d")),
      std::uint256_t(std::string_view("09d56c059bdc3b72809b5eca0cfba145bb35466061e26671b5a4cc0500637724")),
      std::uint256_t(std::string_view("09e2710fcb6ecf68699e7dbdfaa1d86f0de34a9120ff6807bfd8ec02fdf8c114")),
      std::uint256_t(std::string_view("2e5e964691be2fc42126f0d6f57326683c3b081e9b437c5aadb8778382a9bf88")),
      std::uint256_t(std::string_view("1fc56830f50084e7d966ed5aded5d26409273005c4fabc15e177daefc7719363")),
      std::uint256_t(std::string_view("26968f396dc45410090a5cafb93d330ece774bcf69ec1cbc5e41ab9dff3e21ff"))
    };

public:

  template<typename container>
  static std::uint256_t Hash(const container &arr, const std::uint256_t &key)
  {
    std::uint256_t r = key;
    for (const auto &one : arr)
    {
      r = r + one + Mimc7Hash(one, r);
      r = r % Q;
    }

    return r;
  }

  static std::uint256_t Mimc7Hash(const std::uint256_t &in_x, const std::uint256_t &in_k)
  {
    std::uint256_t h = 0;
    for (int i = 0; i < rounds; i++)
    {
      std::uint256_t t;
      if (0 == i)
      {
        t = in_x + in_k;
      }
      else
      {
        t = h + in_k + cts[i];
      }

      std::uint256_t t2 = t * t;
      std::uint256_t t4 = t2 * t2;
      h = t4 * t2 * t;
      h = h % Q;
    }

    return (h + in_k) % Q;
  }

  static std::uint256_t HashBytes(const std::vector<uint8_t> &input)
  {
    constexpr size_t n = 31;
    size_t len = input.size();
    size_t group = len / n;

    std::vector<std::uint256_t> elements;
    for (int i = 0; i < group; i++)
    {
      std::vector<uint8_t> temp(&input[n * i], &input[n * (i + 1)]);
      std::uint256_t one(temp, false);
      elements.push_back(one);
    }

    if (0 != len % n)
    {
      std::vector<uint8_t> temp(&input[n * group], &input[len]);
      std::uint256_t one(temp, false);
      elements.push_back(one);
    }

    return Hash(elements, 0);
  }
};
