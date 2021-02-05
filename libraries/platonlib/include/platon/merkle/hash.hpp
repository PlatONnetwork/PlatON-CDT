#include "platon/fixedhash.hpp"
#include "platon/chain_impl.hpp"
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

struct MiMC {
  static h256 Hash(std::span<const h256*> elements) { return h256{}; }
};

}
}
}
