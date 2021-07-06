#pragma once

#include <algorithm>
#include <array>
#include <iterator>
#include <vector>
namespace platon {
namespace hash {
namespace rescue {
struct Bn256 {
  constexpr static std::array<std::uint256_t, 135> ROUNDCONSTANTS{
      std::uint256_t(std::string_view(
          "2e827c42545887ff44c8e94bc429a6b9fba992c691596d3e97216a720f7b48ac")),
      std::uint256_t(std::string_view(
          "03c75f89abe4ca0baf0d7fa782a1c0b7d06953671bbbf7de43257c6dc6e54dca")),
      std::uint256_t(std::string_view(
          "19774ac9566bf383567f7362d1c8e62d4bba886dec26174637c734be0df06cc0")),
      std::uint256_t(std::string_view(
          "043d62d812dac074cba7666e4e024bd24b52084db7832ba740cf341f0be88470")),
      std::uint256_t(std::string_view(
          "04e8b099362f0cbf4e86b5ab59b734df49286bf95a0493af4d222847faf89051")),
      std::uint256_t(std::string_view(
          "07e7156a711c0dce97e1bf3c92c66869fc8387c3ba93baabdeb7935e9229ef94")),
      std::uint256_t(std::string_view(
          "1d4418eb293b45bcefea1ab6c2e25773ff151b61e1c717d50bb7cbe1317dc565")),
      std::uint256_t(std::string_view(
          "02cf0956c2566ed0c0483b06b6a17debcdebf16c0858597850bce1a4d10a525c")),
      std::uint256_t(std::string_view(
          "0faa7ae09629bdacd212b1450943445a93ab4e10559a901fb33ff8ead7ded122")),
      std::uint256_t(std::string_view(
          "1020d9f9e31ac33accebdb8e7127dfa69f0cb13d16cea22c2afa39189b8eebc9")),
      std::uint256_t(std::string_view(
          "0a686243a889e0adc19b413a6882897dd0d13fb983e3a6fe355beff21a91d184")),
      std::uint256_t(std::string_view(
          "1c83e8b36d64cc64756872d4c714124768390d6aacee13e616f8eebe854ef9eb")),
      std::uint256_t(std::string_view(
          "2c24954fe8789a1dc0e4612d12d96b278d3eefd1015a7adb9a1d94bf8575f418")),
      std::uint256_t(std::string_view(
          "113b0d59035669b65c2bc5cf6d4dfccbfaf068a8324713b8cb287728eea9fb09")),
      std::uint256_t(std::string_view(
          "1eda7697977e5ff6d190473db97c8129c24a01410747a5953d01ae41f1201fa8")),
      std::uint256_t(std::string_view(
          "06336fa6da86fea1244edba9a602817992db494e8ee4cd1cdd5cef1e31a457ee")),
      std::uint256_t(std::string_view(
          "1cbb120a8d2384f98b5f7dde5c2d9a46c74f3f8f077447275a97feb851e923af")),
      std::uint256_t(std::string_view(
          "292a5b37425d93e01103d182e196c91f6a78de6fc8706045ffbe5e3b4113c475")),
      std::uint256_t(std::string_view(
          "147729a654c70dfd83d0d6734d4eb1a28d5fe971f8fe4b04a923d38611b2ba51")),
      std::uint256_t(std::string_view(
          "11c91cdf81970c31ae841646c8c8af01b5c86f8c553b92432c04375882981f74")),
      std::uint256_t(std::string_view(
          "0833bdaf9311d54b95fab90c3bda39f93a0489121a83da532041a2fa49802fd6")),
      std::uint256_t(std::string_view(
          "24c823de84f3c92052667494f19b36bccbd5dfe3c9d9eee699cd820a4eec27f8")),
      std::uint256_t(std::string_view(
          "2d5e9c028e0d90fbcfcada03edba08a87b08d6ea24850406bf193d370664d429")),
      std::uint256_t(std::string_view(
          "262a0fb1764ca2a69d54f9587e87736f9f7900eb83de841a3baec1956b7bdc69")),
      std::uint256_t(std::string_view(
          "303b15e35ddb721b8876809e847b8412da4211826fd5442bf4380ea05a01d3fe")),
      std::uint256_t(std::string_view(
          "0dad6859822f688dfcd68b0ccc95263884c135210cc7ba2aced542d618cc877f")),
      std::uint256_t(std::string_view(
          "20fb16de97d6fe9025d6e57d1aca4015f8938eed24d309ea45c53a1e44c822e0")),
      std::uint256_t(std::string_view(
          "290804f3f5d08d175efb5f4610aea2f6312cc0fd813a2158ebc8d39383d6bd99")),
      std::uint256_t(std::string_view(
          "14182199a12b196fbed8b6c03136354f72d6cb908e35f38977551ed9a396d271")),
      std::uint256_t(std::string_view(
          "2c23201a5e1e868b224b41ca808f5213bfffe29ac83fbed477032770d188ead5")),
      std::uint256_t(std::string_view(
          "0fa4cadd5eb82dac3aa145b52f72f6e557c728e7b0d6550329cbdbf7803e9291")),
      std::uint256_t(std::string_view(
          "0aa69f987c5d7d59551d8d8149018abcfd349fef82883c4c828fc9c8f7d57d3a")),
      std::uint256_t(std::string_view(
          "2ae7aeee1710fac62f0f7b8fc31bf22e602656e8f4d201d02f45bb5c5a9ea2a7")),
      std::uint256_t(std::string_view(
          "17efbf28294d5f6318a7caede8d58ff01a0283488eaa32534391d65432525a47")),
      std::uint256_t(std::string_view(
          "07d52d080d752529f8bc16e68936c05458b013fd3a2c3ec70c98771efc01b36e")),
      std::uint256_t(std::string_view(
          "09323e13b19f985d0d1a795b1918a7700eb222bb8ebbe4da30187e28036b96e9")),
      std::uint256_t(std::string_view(
          "241e50fb623a7713097a5f45814cd09148c1b678f51d0b3a3bb84fde2bb12cb2")),
      std::uint256_t(std::string_view(
          "2ab7da957ae223437ba66e5fd9bef4ee6630e154bd0358a5f3a2062b6c6fb861")),
      std::uint256_t(std::string_view(
          "128bc77708c998f8fb3cdab49464c1077be69b8d369ea861d1385a6b2691ecab")),
      std::uint256_t(std::string_view(
          "179ee05447dcf2089971a88243ef7a0ed779d91b1ee62012e15575e6e1b3c5b4")),
      std::uint256_t(std::string_view(
          "2b14779395dabf0e13b20f90540f25a7bbf056cf3c68f2cc1e9bafe3bc1220a9")),
      std::uint256_t(std::string_view(
          "27b3090f24d3ec29af7f58bf923f8a131b3c9b21ea57884828d5fedd3dec311c")),
      std::uint256_t(std::string_view(
          "219b7eb539cd8f5e783a1fc07048fce9eb83d1cdc0d3493fc09e56e23348f98f")),
      std::uint256_t(std::string_view(
          "075941e54effafe29b8a0836ceff2c7f11da4b7932d5a9d779cb2e5a3e2da771")),
      std::uint256_t(std::string_view(
          "13db5be2a23de171268477b3c246848d6b40906465a8c1a1a8ac4c5e937fe621")),
      std::uint256_t(std::string_view(
          "086ffc0ed3c8a41b5b882b7982faf2b8f423f8df34d5efc85dd0d387e8cebe44")),
      std::uint256_t(std::string_view(
          "0bae52cf7553fbcb400ac145ca8062a86bbc982784c9f3a3d8a2dd6b29b5306e")),
      std::uint256_t(std::string_view(
          "0c5d0c9822e8064b4764dbaa71ecb67a574ab7be3c3034b61b09c3c9dbda5c12")),
      std::uint256_t(std::string_view(
          "09324dbb4fa19d86b4548778edb2ff17d9d1d82fbd47bcde9e4d8bb0c2719809")),
      std::uint256_t(std::string_view(
          "241d32c2e6a3c0d62c86a8400eb318ea48649f6ae8898f7eaca220cadb16fcb0")),
      std::uint256_t(std::string_view(
          "03b43fcc68db6c0303155b6ab0bc2dd97120e5f714df7fc91ba125d963ab2482")),
      std::uint256_t(std::string_view(
          "0a8381ff49e2715755131c98a26a277f7107e89d2dae8b0e78fe4e977b9aee8d")),
      std::uint256_t(std::string_view(
          "249a4becebe8221baa207413a79a67933d620d64dfbbdc7ce5d34827bfe0b2ad")),
      std::uint256_t(std::string_view(
          "16bb4dafd6d3a0ad48deb10cc96f1e0e39a85226fb82c7ecd4a469f8c651ea2a")),
      std::uint256_t(std::string_view(
          "2571b13b8139341cfbfdfb35331a243e124cf54c7e301dab4221d661efaa3b8a")),
      std::uint256_t(std::string_view(
          "1f082b11fd4514d0dbebadb200384adff84b6d786527f842e5f97307b7a3f66d")),
      std::uint256_t(std::string_view(
          "12b36359c0b3a84656219c6d450a88e9732a6a776a570673a5229918c5bef10a")),
      std::uint256_t(std::string_view(
          "22fa7160d8e1135bd5181d3e2d761f7fd45c042ad4f0d9e60c6df50759afb82f")),
      std::uint256_t(std::string_view(
          "1c3fba7f717ffa81059e88c6bb9d4489aed9600e8ba037bf522e1b45384d8c70")),
      std::uint256_t(std::string_view(
          "1c56fd26e55afa44f2fc6d8f2d508a06676ac05614df9b175da02ece31040d88")),
      std::uint256_t(std::string_view(
          "1df86d2ebe9bd9074b9e857c5d2f746ab70629c22c213c91fa51ff1bd174b475")),
      std::uint256_t(std::string_view(
          "0c3cee4e1a0d78b7e1508406a11515d2f8db2124486e260f13255fe4c43df747")),
      std::uint256_t(std::string_view(
          "2067e59f65bbdd4f072c4a1f15c5d417547cec2fb50ac7cf669e767239575ffc")),
      std::uint256_t(std::string_view(
          "2ef209879e17db4b4270b59c29a5f894a2711b4e29e1ee82ce24796873590e68")),
      std::uint256_t(std::string_view(
          "274fd928d6080deb05be861c58972b786c99736442c34bae4fb7a5cc4d947eb4")),
      std::uint256_t(std::string_view(
          "11979c252ae6ac6c9640e6760af80b50e958b09e921dd03e6389ca890891c913")),
      std::uint256_t(std::string_view(
          "267082663cdcd184ed30a3f2b6c9e0301a9ead5f52b497c793f2eba170ef54e0")),
      std::uint256_t(std::string_view(
          "2d0667da81aea5237be170fecf619ded19b841e3471b9b80a20d19be17981d43")),
      std::uint256_t(std::string_view(
          "2815464fcc9566a3a53a180d6fb8d7c028ea90eacdfc0df2ac71f518ab79a617")),
      std::uint256_t(std::string_view(
          "0bd5a8700ca30de6f9f760e74c6930744fc206d27b4d2ee35d0a2f0e1130deb2")),
      std::uint256_t(std::string_view(
          "18a10c4f55b6311dbbd44ee961ed0dda628c3ebfb2098f4b9098f28e8899bb23")),
      std::uint256_t(std::string_view(
          "28a57a4a66f72150f1b61e47dc04124db7f88ccbcbe852f8d95529ac4ada28bf")),
      std::uint256_t(std::string_view(
          "160e287a5ab0869feb86b55a1754d8978824d2ddf9fb3432feb24196e64e6507")),
      std::uint256_t(std::string_view(
          "1f8d9bc1be9bb169c4f2e882ec0a52c0a11bd7a01eaf65cc47334ebb6afbca12")),
      std::uint256_t(std::string_view(
          "29c392cab71cfb9692295ed3b56bef36cb4d6a636cf2158e968cfd1a854a302c")),
      std::uint256_t(std::string_view(
          "0cf1c91a22e683c1443881d104fdde8161ee66bcb468a667e511dbe0f492293c")),
      std::uint256_t(std::string_view(
          "2ae4874d96c7c84a3d9f6926340abdf1e31555d4f504e9a06dbbcafb211069c7")),
      std::uint256_t(std::string_view(
          "02b0ad5c08cb10395d2d12674834ae08be8e20d512d258c054f90164db4a6572")),
      std::uint256_t(std::string_view(
          "0a497b74786056ba4335749abebdc9ed22dfcf8fc5a425b7496bf1acedd86110")),
      std::uint256_t(std::string_view(
          "29d3c40517092e92ead3e510ca0855c1620d5f9152d5d524c43ab0fb5d3d855b")),
      std::uint256_t(std::string_view(
          "1205185af96147aa3e3d7ab586c0998006ff5265952071f893b508544b13c7f5")),
      std::uint256_t(std::string_view(
          "10cfeadc8cd3196a08c0624e279c9b4a46755f161bc914764f97a8319515191e")),
      std::uint256_t(std::string_view(
          "27792e3c522f9f78a9296d9172207a644856f607df91603b9dba0f63d64d4f04")),
      std::uint256_t(std::string_view(
          "276c221946ed59247fccd06f4f2b8e1c075d7ba2f1199ac1009b2637ba7e95ba")),
      std::uint256_t(std::string_view(
          "07f97f0b29d227dc5e467417682e604012474e29195ddb2995935933e2c145c3")),
      std::uint256_t(std::string_view(
          "23999e25e79086ed8a533932ad49da9406acf7aa488b9457967301514560af54")),
      std::uint256_t(std::string_view(
          "23c70014ae1555c31bcedbb718e6da69897d74d8cfe000e094f8b670642f7ab7")),
      std::uint256_t(std::string_view(
          "2af92d6b81674d250c6f66621bbd9f8b4449a21c18ffefd9032b07880433a60d")),
      std::uint256_t(std::string_view(
          "1baabd1b5e38855f595d16ea11cc2827e8586c7ce0a50ab7e318b093da5b4984")),
      std::uint256_t(std::string_view(
          "2d49d4ed96f91184e58a897daf48902b1c96ad68b7799931364512646346ad82")),
      std::uint256_t(std::string_view(
          "0241072649eb7fc60f4214df7d6fd80048d8bf97419453c26c5d3f576a46046a")),
      std::uint256_t(std::string_view(
          "12adde3c8f4411488d475eb14245393580080fadb544369c91a284362da953f5")),
      std::uint256_t(std::string_view(
          "2d9ce38e088cc3d8e5d783d78ef557ac3f6b29c5552572808d2eef43342f9077")),
      std::uint256_t(std::string_view(
          "1d96452671994e1602a77c6a6268ffd7ccdbf39e527d19642dca86dfb1769085")),
      std::uint256_t(std::string_view(
          "01da79fbce33ec6ddae14375559290f36db35163021169b08c68318cf351df2d")),
      std::uint256_t(std::string_view(
          "2b9589381bf636ca778f7d40335fb169fb7e7957afd8c54ba4bb66bb89d7efce")),
      std::uint256_t(std::string_view(
          "15438d653cefa32fbc3fbccdc3da2abc063d75aee95f4b9ab822db035f34161c")),
      std::uint256_t(std::string_view(
          "11a38e9b2f0a2f8230820ac0a75846c828e8556b2d4bb0c719e8689c6cb2d742")),
      std::uint256_t(std::string_view(
          "20aebd8eaa2ba10c40c2039f5278de49cea826c250b75f315f7cca82fe1bcccf")),
      std::uint256_t(std::string_view(
          "2805f77ba0f90bfbf214effe5faa7f9a3bee2eb6f19719360aec0e609cdddbb7")),
      std::uint256_t(std::string_view(
          "1f35fd366f894caebdfb7aecc3a471d014d524aa6274bf7dd7c98703f632b7f1")),
      std::uint256_t(std::string_view(
          "1138a44eb81c2ded4d764b2d8126f9eced927c29716e8a23a18d268ff3e364bd")),
      std::uint256_t(std::string_view(
          "09ea16c15205adf61033488bdb59d17e7613f90d39f6e4b7a97dea7792068575")),
      std::uint256_t(std::string_view(
          "18642ea66de5b1efd1682e8a5da2e030f97ac8e9d28a8fec9fcac2acf123ad5a")),
      std::uint256_t(std::string_view(
          "2d00b31b568c5b7305df5d853097ea0a780034e5446db453b433e078cf806ce9")),
      std::uint256_t(std::string_view(
          "0bdbdb7dbc5f742e40459e321270221c3ac0fe765e18d1c83c1f12e0e4276ab0")),
      std::uint256_t(std::string_view(
          "27f0f1e9106b61d0a6be2bce5e1e395a1c3fa169bee228839ccbe21ffeec9685")),
      std::uint256_t(std::string_view(
          "19a6c9fc9430f3f39ad62e3b6faba355d314bf5c9be58315bd73b94454058983")),
      std::uint256_t(std::string_view(
          "0117d38e94416d29bdabf94450b5681f7604b17937f1b24d3d18baf5cd2913b3")),
      std::uint256_t(std::string_view(
          "0561af313be692a3b6bab41cbf3ba2ddbe27bc1c4107a602cd7b74924e4fcbe5")),
      std::uint256_t(std::string_view(
          "17e0bf6748e4591533d3bf82f29eda7c8c23e1b13fb5359576e83983bc7ffd51")),
      std::uint256_t(std::string_view(
          "20301abe834d1352fb22a83575e37310b971ff87fd5f8b7c3e4a60e52d77e2bd")),
      std::uint256_t(std::string_view(
          "2af84be1a167caccf34fe68d6912d28286bc97ef1f595cff131378dbd2e554ce")),
      std::uint256_t(std::string_view(
          "05ea371f188b0d76868773b89838cc899891790098888b0961cad2561c40306f")),
      std::uint256_t(std::string_view(
          "0cf6ce4d9373be9539fb4421ad10b683aee4924adbe827286f4ceac1e4c67842")),
      std::uint256_t(std::string_view(
          "0aca25eb78a73109d86dff873e22f32df2b16ba89976521eb602ff31b48ce6a3")),
      std::uint256_t(std::string_view(
          "1377ac6a61a13e8b5d0b03956cf32d499907a6664bfeea10199d8b7642e5234f")),
      std::uint256_t(std::string_view(
          "0117c5b7fed49913c1a4a3b0359c1ffd893a39896dd85c08628455025e36643a")),
      std::uint256_t(std::string_view(
          "1f51935bcdc7c51b8f74a335bebaef3ae69e06b0db01351c62954a15d1e4096b")),
      std::uint256_t(std::string_view(
          "2817ee19e12e7566c4221c3527563f333ce95085a67b2fc524fd426b9ec131d7")),
      std::uint256_t(std::string_view(
          "0959c2acc31d5651446cec07dcadb4a07eed276dbb9df45c02389d974e1c4373")),
      std::uint256_t(std::string_view(
          "0312c8f6f38ee402392f6afa130b34a4a27ccf79495043dc24a1f4bb8b09d602")),
      std::uint256_t(std::string_view(
          "27cf6996f2db864b41af1ee4dc87a9a51e3b1c4433ccb2e76d4e86de1d2b209c")),
      std::uint256_t(std::string_view(
          "0c9b46ee380ed1497bb40a2400f32552a4e9ef8f8ca2caa54e41af45801fe0bb")),
      std::uint256_t(std::string_view(
          "25c787129ae25108aead79cc75d304f15e23f1e5bf774f975680691966813e99")),
      std::uint256_t(std::string_view(
          "0e1982f2eaff67328c5d8437f91432be5f546c3db83a3b3d346529120907f99f")),
      std::uint256_t(std::string_view(
          "2d3d5fc23723cf7eb9bfa2049585623029ca9759728046840123dffe86f6e950")),
      std::uint256_t(std::string_view(
          "06befe53a900cdcc215e89b77b6d51d72486c5273b2ae704ca10acc3c4c22351")),
      std::uint256_t(std::string_view(
          "0345abcbd6380b14a5938f4346683fae5749c1c6934444e75cdf7300c90e8e69")),
      std::uint256_t(std::string_view(
          "04826ffdef990240d507d26e2789d095729bc9a0a94508d261c36fe53a549bee")),
      std::uint256_t(std::string_view(
          "1d6dfc9dcf49c371996f6bdf2d2ec503d2b0b4b0775bfce0d988ac99c94178fc")),
      std::uint256_t(std::string_view(
          "184d027e3c7bdb1f245ef2f98b2b1adf4cd54b54cbdd4fb2b2f9d8342d647b52")),
      std::uint256_t(std::string_view(
          "2062808e98d02369b3360b737d446985455e0a93155a0eec12376048d61650de")),
      std::uint256_t(std::string_view(
          "2de847a34da34f6c41d66683572814b1e70c66deafe51b0bf949abde45b9b646")),
      std::uint256_t(std::string_view(
          "164ed456b62365fbf715619f80da62933b86aed7522914928a7cfdc1ba3d7645"))};

  constexpr static std::array<std::uint256_t, 9> MDSMATRIX{
      std::uint256_t(std::string_view(
          "2bec9c48301bdaaad9c72a3d8c4d363ac8be8189ccea49a4c0bd8c37d1c6b3e3")),
      std::uint256_t(std::string_view(
          "1f069c8a0ff22a1e62469e5f812fe7aca34de4c88515d84c9a1af9e21f5121a8")),
      std::uint256_t(std::string_view(
          "0d5365c702a1d156ecb069c1a5b6f3fefa94d552d01f19e0cea4ba91e4537c0b")),
      std::uint256_t(std::string_view(
          "1e4038a58dfdab6147e951763747a13f5e5599737bfa3c30d4548a366740959e")),
      std::uint256_t(std::string_view(
          "07cf3d4093243419a52e08fa9416458849e98b266a981933f96649f53f7824e3")),
      std::uint256_t(std::string_view(
          "116b9e8295f0086e020f035edc4ec2b5e65187597edb3d37de19589968b8d95a")),
      std::uint256_t(std::string_view(
          "1e2a87b6b95d6c53a4ef91c0f455ebda2d3b80bff560df8afc804813c15542fb")),
      std::uint256_t(std::string_view(
          "16e10361cbf35fe8e9c30527df038594e611b03897f65996a6986e51cfe3a96b")),
      std::uint256_t(std::string_view(
          "1c794b13d4b66d9883849539f556b020117666918dae1210cc4c713c4137c980"))};

  constexpr static std::uint256_t Q = std::uint256_t(std::string_view(
      "30644e72e131a029b85045b68181585d2833e84879b9709143e1f593f0000001"));

  constexpr static uint32_t PADDINGVALUE = 1;

  constexpr static uint32_t ROUNDS = 22;

  constexpr static uint32_t RATE = 2;

  constexpr static uint32_t WIDTH = 3;

  constexpr static uint32_t ALPHA = 5;

  constexpr static std::array<uint64_t, 4> ALPHAINV = {
      14981214993055009997ul, 6006880321387387405ul, 10624953561019755799ul,
      2789598613442376532ul};
};

template <typename Parameter>
class Rescue {
 public:
  static std::uint256_t Hash(const std::vector<std::uint256_t> &input) {
    std::array<std::uint256_t, Parameter::WIDTH> state;
    state[Parameter::WIDTH - 1] = input.size();

    // padding
    std::vector<std::uint256_t> paddingInput = input;
    while ((paddingInput.size() % Parameter::RATE) != 0) {
      paddingInput.push_back(Parameter::PADDINGVALUE);
    }

    // Group operation
    auto it = paddingInput.begin();
    int absorbtionCycles = paddingInput.size() / Parameter::RATE;
    for (int i = 0; i < absorbtionCycles; i++) {
      for (int j = 0; j < Parameter::RATE; j++) {
        std::uint512_t temp = *it++;
        state[j] = (temp + state[j]) % Parameter::Q;
      }

      RescueMimc(state);
    }

    return state[0];
  }

 private:
  static void RescueMimc(std::array<std::uint256_t, Parameter::WIDTH> &state) {
    const std::array<std::uint256_t, Parameter::WIDTH>& tempRoundConstants =
        RoundConstants(0);
    for (int i = 0; i < Parameter::WIDTH; ++i) {
      std::uint512_t temp = tempRoundConstants[i];
      state[i] = (temp + state[i]) % Parameter::Q;
    }

    for (uint32_t i = 0; i < 2 * Parameter::ROUNDS; i++) {
      if ((i & uint32_t(1)) == 0) {
        Sbox0(state);
      } else {
        Sbox1(state);
      }

      std::array<std::uint256_t, Parameter::WIDTH> tempMdsApplicationScratch =
          RoundConstants(i + 1);

      for (int i = 0; i < Parameter::WIDTH; ++i) {
        std::uint512_t scalarResult = ScalarProduct(state, MdsMatrixRow(i));
        tempMdsApplicationScratch[i] =
            (scalarResult + tempMdsApplicationScratch[i]) % Parameter::Q;
      }

      state = tempMdsApplicationScratch;
    }
  }

  static bool BitIterator(const std::array<uint64_t, 4> &info, int i) {
    int n = 64 * 4;
    n -= i;

    int part = n / 64;
    int bit = n - (64 * part);

    return (info[part] & (uint64_t(1) << bit)) > 0;
  }

  static void Sbox0(std::array<std::uint256_t, Parameter::WIDTH> &state) {
    for (auto &one : state) {
      std::uint256_t res = 1;
      bool foundOne = false;

      for (int i = 1; i <= 256; i++) {
        bool bitIter = BitIterator(Parameter::ALPHAINV, i);

        if (foundOne) {
          std::uint512_t temp = res;
          res = (temp * temp) % Parameter::Q;
        } else {
          foundOne = bitIter;
        }

        if (bitIter) {
          std::uint512_t temp = res;
          res = (temp * one) % Parameter::Q;
        }
      }

      one = res;
    }
  }

  static void Sbox1(std::array<std::uint256_t, Parameter::WIDTH> &state) {
    if constexpr (5 == Parameter::ALPHA) {
      for (auto &one : state) {
        std::uint512_t quad = one;
        quad = (quad * quad) % Parameter::Q;
        quad = (quad * quad) % Parameter::Q;
        one = (quad * one) % Parameter::Q;
      }
    } else if constexpr (3 == Parameter::ALPHA) {
      for (auto &one : state) {
        std::uint512_t quad = one;
        quad = (quad * quad) % Parameter::Q;
        one = (quad * one) % Parameter::Q;
      }
    } else {
    }
  }

  constexpr static std::array<std::uint256_t, Parameter::WIDTH> RoundConstants(
      int i) {
    uint32_t start = Parameter::WIDTH * i;

    std::array<std::uint256_t, Parameter::WIDTH> result;

    for (int i = start, j = 0; j < Parameter::WIDTH; j++) {
      result[j] = Parameter::ROUNDCONSTANTS[i + j];
    }

    return result;
  }

  constexpr static std::array<std::uint256_t, Parameter::WIDTH> MdsMatrixRow(
      int i) {
    uint32_t start = Parameter::WIDTH * i;

    std::array<std::uint256_t, Parameter::WIDTH> result;

    for (int i = start, j = 0; j < Parameter::WIDTH; j++) {
      result[j] = Parameter::MDSMATRIX[i + j];
    }

    return result;
  }

  static std::uint256_t ScalarProduct(
      const std::array<std::uint256_t, Parameter::WIDTH> &state,
      const std::array<std::uint256_t, Parameter::WIDTH> &by) {
    std::uint256_t result;
    for (int i = 0; i < Parameter::WIDTH; i++) {
      std::uint512_t temp = state[i];
      temp = (temp * by[i]) % Parameter::Q;
      result = (temp + result) % Parameter::Q;
    }

    return result;
  }
};
}
}
}