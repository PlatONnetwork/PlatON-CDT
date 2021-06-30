#include "platon/fixedhash.hpp"
#include "platon/chain_impl.hpp"
#include "platon/hash/mimc.hpp"
#include "platon/hash/poseidon.hpp"
#include "platon/hash/rescue.hpp"

#include "platon/bigint.hpp"
#include <span>
namespace platon {
namespace merkle {
namespace hash {

struct Sha256 {
  static h256 Hash(std::span<const h256*> elements) {
    std::vector<byte> data;
    data.resize(32 * elements.size());
    for (size_t i = 0; i < elements.size(); i++) {
      memcpy(data.data()+i*32, elements[i]->data(), elements[i]->size);
    }
    return platon_sha3(data);
  }
};

struct Poseidon {
  static h256 Hash(std::span<const h256*> elements) {
    size_t len = elements.size();
    const uint8_t** inputs = (const uint8_t**)malloc(len);
    for (int i = 0; i < len; i++) {
      inputs[i] = elements[i]->data();
    }
    h256 hash;
    ::poseidon_hash(1, inputs, len, hash.data());
    return hash;
  }
};

}
}
}
