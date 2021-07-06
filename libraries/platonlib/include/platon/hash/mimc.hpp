#pragma once
#include <array>
#include <cstring>
#include <deque>
#include <vector>
#include "platon/bigint.hpp"
#include "platon/chain.hpp"
namespace platon {
namespace hash {
namespace mimc {

class Mimc {
 private:
  static constexpr int rounds = 91;
  static constexpr std::string_view seed = "mimc";
  static constexpr std::uint256_t q = std::uint256_t(std::string_view(
      "30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001"));
  static constexpr std::array<std::uint256_t, 91> cts{
      std::uint256_t(),
      std::uint256_t(std::string_view(
          "2e2ebbb178296b63d88ec198f0976ad98bc1d4eb0d921ddd2eb86cb7e70a98e5")),
      std::uint256_t(std::string_view(
          "21bfc154b5b071d22d06105663553801f858c1f231020b4c291a729d6281d349")),
      std::uint256_t(std::string_view(
          "126cfa352b0e2701442b36e0c2fc88287cfd3bfecce842afc0e3e78d8edb4ad8")),
      std::uint256_t(std::string_view(
          "0309d7067ab65de1a99fe23f458d0bc3f18c59b6642ef48afc679ef17cb6928c")),
      std::uint256_t(std::string_view(
          "194c4693409966960be88513cfe32987c125f71398a782e44973fb8af4798bd8")),
      std::uint256_t(std::string_view(
          "05a849684bc58cc0d6e9f319b4dae26db171733bf60f31d978e41d09a75a6319")),
      std::uint256_t(std::string_view(
          "18bd4dae5134538bd2f90d41bbb1e330b2a8286ba4a09aca3fbbdcf932534be5")),
      std::uint256_t(std::string_view(
          "0736c60cd39fd1649d4845b4f9a6ec9baca89fb2de0a3d7eeabe43504b5607fa")),
      std::uint256_t(std::string_view(
          "25a6971a9d2c1de9f374378d8f61492b1bd3c46584c076a76c43c3cd1a747512")),
      std::uint256_t(std::string_view(
          "0a3373d15fa6dce221f83226c02d41f8aea5cfc6da4c9f4981ada1bd4b50f56e")),
      std::uint256_t(std::string_view(
          "2b70028e2bf4e008e22eddb78d4190d73c289dc6445b3f64e15f8bd0ec02c672")),
      std::uint256_t(std::string_view(
          "0b24ef461a71eed93dd366342f9ca4eebb749c8a5a6057c801d538c7c0666ba4")),
      std::uint256_t(std::string_view(
          "05d1e0ac576d1ec814b621516339ae1a291c7df36b5fd6cf0b4e3c9cd25e3072")),
      std::uint256_t(std::string_view(
          "271cfbf88e9744b8596e7e2d6875c8005d0e62014010ac35e95a7ce2390bc50f")),
      std::uint256_t(std::string_view(
          "196309f1d170d741ab1ce90c39772017fb7cdec78c37882b98a6b56956c13def")),
      std::uint256_t(std::string_view(
          "127c1116c575c03c7f6d83417d8c1b3808f92ee16924a54094bf094721e9e4f5")),
      std::uint256_t(std::string_view(
          "1bff78047ee67d38a54fdc540f9a2ba07f63489acd36425f1ae210ac329826f5")),
      std::uint256_t(std::string_view(
          "06c7dc7bbae615fcf1896f2b8db7d92c05dc1ea1c8134e9db6fd588672c53e9a")),
      std::uint256_t(std::string_view(
          "12df78cba175ef76dbfcc9c785926bb3949a87ec7533e2559a27a64b91cebba5")),
      std::uint256_t(std::string_view(
          "2bd4cdc962e3da62cb3c96f7c428a9b0d518bfa7ce26f8fce7a6af769afb6540")),
      std::uint256_t(std::string_view(
          "24edd3847febbe44c4cc390246e3379b47fd01a030d0cd0b4fcf7fbd1cabfe58")),
      std::uint256_t(std::string_view(
          "1ce065d2c2561bb573e4cf4259d3b0b0e9eacb447751c62b77d0bc5e4e3c7d15")),
      std::uint256_t(std::string_view(
          "18053e9f0d45f9eefbda135bfd39329e34837e633565c314fb9030b9db7381bb")),
      std::uint256_t(std::string_view(
          "162ffa8742138bbe516168bf86ec78b1ad1e8b535ac455a7cfbb22c13f9c5a9e")),
      std::uint256_t(std::string_view(
          "079eea42e16ac6442ca82623fc0e8d9ad3996a47a8013ea9cb73858ca42b7159")),
      std::uint256_t(std::string_view(
          "0a49af2bbe11b05bd02a69a47b1bad5b2170407ada21142f06e4e109de88a1b6")),
      std::uint256_t(std::string_view(
          "12c34eebbaa69cccc36929e8f4a6e40771e153ff77943da55c4fc860537b733a")),
      std::uint256_t(std::string_view(
          "008de5ac6b4e359335b6fce58dc0e5e43fd2aefd86bac35abe579b8cace5dbc8")),
      std::uint256_t(std::string_view(
          "04a6e988b50d915734bf3296d83057ffe6a550f8987e4597bee7d333cd24a865")),
      std::uint256_t(std::string_view(
          "24112633926cfc6028fa2ffd9f090b1e5428a0a87d7118356e48b5d470449217")),
      std::uint256_t(std::string_view(
          "0d56329982f3df38a3f19fb814c3013f419ba0eb8403b27c0c0e75c6fe1cf468")),
      std::uint256_t(std::string_view(
          "1f01ef80763c95f53c434164493d9673aeef290bf1aa1997d677b557b9692e8a")),
      std::uint256_t(std::string_view(
          "105c5257f801527e60b0361c00075b5a79d2dc6821d8a1258d906ed453c7e7be")),
      std::uint256_t(std::string_view(
          "03db505a0c32cb61ca099389c2180e1c83827fb41d9fed84d88766df44c63079")),
      std::uint256_t(std::string_view(
          "1262e738f38db6c79d24d9727294421cd95afa24f4700c1323ab83c3a06ace32")),
      std::uint256_t(std::string_view(
          "0ee68c3e38c194033994c0d4d7bde35bfafa35b22a95f915f82c5a3b0422bd9a")),
      std::uint256_t(std::string_view(
          "2ee5427bd20c47f8d2f0aa9e6419f7926abcd5965084292ae54dd780077e6902")),
      std::uint256_t(std::string_view(
          "1e542d31d2a381792e0a9241c46229a22fd9382443e423a0e419d0feb58656af")),
      std::uint256_t(std::string_view(
          "0ba39f01462ab6a7cf621952752fcde48677d7f32df47e940eacf4954c5ef632")),
      std::uint256_t(std::string_view(
          "29c00b058c17800146bdc06b1e73ff5d0ff53df96f8463818c0572d11fcaf88b")),
      std::uint256_t(std::string_view(
          "0b6200895b60a6c6794fcf1c2b1b15d03a713c905a8ba1f1315f7501fe1a50b8")),
      std::uint256_t(std::string_view(
          "2bc639b1b85d731f62d2c6f391d4498e392cb75edcbd5c4c0fa8b26d32d68a12")),
      std::uint256_t(std::string_view(
          "2a89f38e6440ce641127046b67d8e615f14503d72d76bf3c703a01d1463a8445")),
      std::uint256_t(std::string_view(
          "1750ede7eeeb4edd7838b67fac6d250a54055eeead10e69b3a6e1f076ca87868")),
      std::uint256_t(std::string_view(
          "0c2d65084bead2a743115be5329d5458d29802081f6f9dac4165c42651f9be2b")),
      std::uint256_t(std::string_view(
          "28303e2d834e16e1fe33c9ab726a3e75dd0dad9bfea1a43267199e1f243993fb")),
      std::uint256_t(std::string_view(
          "2b572811ca34ea5110d10772e4ced362ebefd7cd1e1884b769e9435914efc5e5")),
      std::uint256_t(std::string_view(
          "17521ca5799fe2ea82c67c0a8d0863b5eec0ef9b703e195dd402b7008b53f6b4")),
      std::uint256_t(std::string_view(
          "0407e54b96a5b63c609fa3797b223c73d260a365ad58b25891a5660272096bd5")),
      std::uint256_t(std::string_view(
          "1a3cd155b03c7d33cc8222c997424bc14069e2edbf4b8aa564c9e5832bdace91")),
      std::uint256_t(std::string_view(
          "296255b5e697e517c502ba49b18aaad89514a490a02e7a878b5d559841b93fbd")),
      std::uint256_t(std::string_view(
          "174835801a1f1525b4c21853b965c5048af465e9f79de9d16748c67953da79a7")),
      std::uint256_t(std::string_view(
          "2d4afed7a708e5972e84d766292f2c841c5d8570961074d59ad3f51e9369a597")),
      std::uint256_t(std::string_view(
          "1c0eb06744c9866e271cd29a7f17f72964faba3cd088b95e73dcce9d92c79ba6")),
      std::uint256_t(std::string_view(
          "26705e7e4f23a7d786ad1786b353a2f8b82269c7b58ab70d7b93f41685d34d45")),
      std::uint256_t(std::string_view(
          "04e674d88b90b1188353106ae25c0447acace9dc6d62cfe7fec2d7993dfd7a22")),
      std::uint256_t(std::string_view(
          "0df3335da13ff46f65095f975d157886241aeccff38fd9bba92644f8969d7e09")),
      std::uint256_t(std::string_view(
          "2dfff62b9282ec05b1fa44479a6e9debe9ac631813d2b10e44b9e0fe19e4d4ee")),
      std::uint256_t(std::string_view(
          "08ece248fe1ce1cd705699b5cd07c990ec27721bab59b657bb138e487ee6694d")),
      std::uint256_t(std::string_view(
          "2c1ab81db607ba76dbf71f48752c856bf183044981c3b6d1fd31b179a078f571")),
      std::uint256_t(std::string_view(
          "01de6f8886868e351bf4caad293bd86ed29ef63810e15cb809542e01bfbbcb88")),
      std::uint256_t(std::string_view(
          "23dd8b576fa286331864d63c77fd82fa61da717533821b9382617ebd54abeb46")),
      std::uint256_t(std::string_view(
          "169f2c8e515b2cee8d183991c3712736001a7f92fb34c3e3f532dec373aacbfb")),
      std::uint256_t(std::string_view(
          "0ecf89b898e2deca99ae5108d271f1fa92e5018c1ac899d554dc1dfa35ceb0a0")),
      std::uint256_t(std::string_view(
          "0dc0d6e76afba377dd693ed4c47a4f9fee7a88d1df5df62fd06f2f87b81de1c8")),
      std::uint256_t(std::string_view(
          "0d8d08571539c68a37dad2a6638291d323948e57a0189a7be2ec14d89308bb6d")),
      std::uint256_t(std::string_view(
          "17d170e737533e922c934f79bad3c28f85ef14b21c7354000298cee876977a44")),
      std::uint256_t(std::string_view(
          "09ed630d4088d7acaa34064515c1cb368ed405c4ded26df38652d290b26f6aff")),
      std::uint256_t(std::string_view(
          "2b5381943dd4c43bd059a4747b72fc116f099c46004dc811ddb440f7ee69701e")),
      std::uint256_t(std::string_view(
          "01da34e987e965c368ec0252e97db8bfb78668db369cdf6c70f7e02b5bd52b3b")),
      std::uint256_t(std::string_view(
          "1a18c896f124cd4821fbe08ac680b78362c15344619cef072874f43799b89f23")),
      std::uint256_t(std::string_view(
          "168dbaf0eae2cfe96f6b340bfd4922c1c41317bfff69613b81d9722e34059f20")),
      std::uint256_t(std::string_view(
          "1dfd587726ec442565eb47fc0234740634b6562d1b60192947140b8670aa4014")),
      std::uint256_t(std::string_view(
          "147a904bcd17a3f66ebd75b2c1279507001e602842a047929fd119d31edf3924")),
      std::uint256_t(std::string_view(
          "00621164e8b17a476172ee2aabd9a1a67ecc05f926bec5bbaceb7524616e1166")),
      std::uint256_t(std::string_view(
          "280fcce91f920b6487ee3e6a838abbc1f7eb44e4853b22d067a56f5e908499b9")),
      std::uint256_t(std::string_view(
          "2d49d03ab6b741495e4d7cbe87ea6cf0f06aea86f528d13d57f6a05e4c868d0b")),
      std::uint256_t(std::string_view(
          "2a59b6e410852d96661479179081af38f478b7603eb3e4f231f99633d826cde9")),
      std::uint256_t(std::string_view(
          "1a7783fa9ff7b36d38aeb75e65cfc88260b70d4600b51ab5745e5fe1dc35d9b1")),
      std::uint256_t(std::string_view(
          "286d1e7e039fa286d1bd8fe69e175ecad61693cc1f55044847191bae2ff344b2")),
      std::uint256_t(std::string_view(
          "0fa108dbe8e14e8c53093f9aaf1f989dabb3dc026ffecb049d3d6b4b2c9b8077")),
      std::uint256_t(std::string_view(
          "0e4b25635fa58150829c3e832c4361bfa7edfdf40b0514c00dd3a7338131f193")),
      std::uint256_t(std::string_view(
          "23b0ea71b8bbd3cb62b741e525f5c8b35cbfed820aaf1234d03a4655cdf71039")),
      std::uint256_t(std::string_view(
          "2aced572dbfd2664569030fcf391019702f79cbfbe380714894fbfc785dad03f")),
      std::uint256_t(std::string_view(
          "03c36b340d12daf2422febd15a4521f351459057c2affd6816c67fa38b3cc34d")),
      std::uint256_t(std::string_view(
          "17d64c030f29369c09ffd529c7532b84228e69ef6dd9d9dab603ba86cb9254e7")),
      std::uint256_t(std::string_view(
          "095050333e4136e4c73b4101ab008bf625a73c51afd5e77f99c606ca7ace63d7")),
      std::uint256_t(std::string_view(
          "10ca0fd2a95bc198763d375f566182463e0c92ea122df6485f1c4e5a9769b32c")),
      std::uint256_t(std::string_view(
          "29f63c935efe224e235d5b49b88578a97b25c739a342d4a0d908b98ef757db61")),
      std::uint256_t(std::string_view(
          "1e1289b8eff2d431b178bc957cc0c41a1d7237057b9256fd090eb3c6366b9ef5"))};

 public:
  static std::uint256_t Hash(const std::vector<std::uint256_t> &arr,
                             const std::uint256_t &key) {
    std::uint512_t r = key;
    for (const auto &one : arr) {
      std::uint256_t h = Mimc7Hash(one, r);
      r = (r + one) % q;
      r = (r + h) % q;
    }

    return r;
  }

  static std::uint256_t Mimc7Hash(const std::uint256_t &in_x,
                                  const std::uint256_t &in_k) {
    std::uint512_t r;
    for (int i = 0; i < rounds; ++i) {
      std::uint512_t t;
      if (0 == i) {
        t = in_x;
        t = (t + in_k) % q;
      } else {
        t = (r + in_k) % q;
        t = (t + cts[i]) % q;
      }

      std::uint512_t t2 = (t * t) % q;
      std::uint512_t t4 = (t2 * t2) % q;

      r = (t2 * t4) % q;
      r = (r * t) % q;
    }

    return (r + in_k) % q;
  }

  static std::uint256_t HashBytes(const std::vector<uint8_t> &input) {
    constexpr size_t n = 31;
    size_t len = input.size();
    size_t group = len / n;

    std::vector<std::uint256_t> elements;
    for (int i = 0; i < group; i++) {
      std::vector<uint8_t> temp(&input[n * i], &input[n * (i + 1)]);
      std::uint256_t one(temp, false);
      elements.push_back(one);
    }

    if (0 != len % n) {
      std::vector<uint8_t> temp(&input[n * group], &input[len]);
      std::uint256_t one(temp, false);
      elements.push_back(one);
    }

    return Hash(elements, 0);
  }
};
}
}
}