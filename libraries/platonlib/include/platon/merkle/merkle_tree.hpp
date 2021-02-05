#pragma once
#include <span>
#include "platon/assert.hpp"
#include "platon/name.hpp"
namespace platon {
namespace merkle {

/**
 * @brief Merkle tree complete implementation

 *
 * @tparam MerkleName The name of the merkle tree, the name of the tree should
 be unique
 * within each contract.
 * @tparam H hash algorithm name, eg. MiMC, Sha256
 * @tparam Levels Merkle tree height
 *
 * Example:
 * @code
 * platon::merkle::MerkleTree<"s"_n, platon::merkle::hash::Sha256, 32> merkle;
 * @endcode
 */
template <Name::Raw MerkleName, typename H, unsigned Levels>
class MerkleTree {
 public:
  MerkleTree() : max_leaf_count_(1ULL << Levels) {
    static_assert(Levels > 1 && Levels <= 32);
    GetCur();
  }

  MerkleTree(const MerkleTree &) = delete;
  MerkleTree &operator=(const MerkleTree &) = delete;
  ~MerkleTree() { SetCur(); }

  /**
   * @brief Insert a leaf node, if the leaf maximum capacity is exceeded, the
   * assertion is triggered and the contract is terminated
   *
   * @param leaf leaf node hash value
   * @return <h256, u128> root hash & offset
   *
   * Example:
   *
   * @code
   * platon::merkle::MerkleTree<"s"_n, platon::merkle::hash::Sha256, 32> merkle;
   * merkle.Insert(h256{});
   * @endcode
   */
  std::pair<h256, u128> Insert(const h256 &leaf) {
    u128 offset = cur_;
    platon_assert(offset < max_leaf_count_);
    leaves_.Set(0, offset, leaf);
    h256 new_root = UpdateTree();
    cur_ = offset + 1;
    return {new_root, offset};
  }

  /**
   * @brief Get the leaf node of the specified height
   *
   * @param depth merkle tree height
   * @return leaf node hash
   *
   */
  h256 GetLeaf(u128 depth, u128 offset) const {
    return GetUniqueLeaf(depth, offset);
  }

  /**
   * @brief Get merkle proof
   *
   * @param index leaf node index
   * @return merkle path
   *
   */
  std::pair<std::vector<h256>, std::vector<bool>> GetProof(u128 index) const {
    std::vector<h256> path;
    path.resize(Levels);
    std::vector<bool> address_bits;
    address_bits.resize(Levels);
    for (size_t depth = 0; depth < Levels; depth++) {
      address_bits[depth] = index % 2 == 0 ? false : true;
      if (index % 2 == 0) {
        path[depth] = GetLeaf(depth, index + 1);
      } else {
        path[depth] = GetLeaf(depth, index - 1);
      }
      index = index / 2;
    }
    return {path, address_bits};
  }

  /**
   * @brief Verify merkle proof
   *
   * @param leaf leaf node hash value
   * @param path merkle path
   * @param address_bits address bits
   * @return merkle root after verification
   *
   */
  h256 VerifyPath(const h256 &leaf, const std::vector<h256> &path,
                  const std::vector<bool> &address_bits) const {
    h256 item = leaf;
    for (size_t depth = 0; depth < Levels; depth++) {
      if (address_bits[depth]) {
        item = HashLeftRight(path[depth], item, kIVS[depth]);
      } else {
        item = HashLeftRight(item, path[depth], kIVS[depth]);
      }
    }
    return item;
  }

  /**
   * @brief Verify merkle proof
   *
   * @param leaf leaf node hash value
   * @param path merkle path
   * @param address_bits address bits
   * @return merkle root after verification
   *
   */
  bool Verify(const h256 &leaf, const std::vector<h256> &path,
              const std::vector<bool> &address_bits) const {
    return VerifyPath(leaf, path, address_bits) == GetRoot();
  }

  /**
   * @brief Get the current Merkle root
   *
   * @return merkle root
   *
   */
  h256 GetRoot() const { return leaves_.Get(Levels, 0); }

 private:
  h256 HashLeftRight(const h256 &left, const h256 &right,
                     const h256 &iv) const {
    std::array<const h256 *, 3> array = {&left, &right, &iv};
    return H::Hash(array);
  }

  h256 UpdateTree() {
    u128 current_index = cur_;
    h256 leaf1;
    h256 leaf2;
    for (size_t depth = 0; depth < Levels; depth++) {
      u128 next_index = current_index / 2;
      if (current_index % 2 == 0) {
        leaf1 = leaves_.Get(depth, current_index);
        leaf2 = GetUniqueLeaf(depth, current_index + 1);
      } else {
        leaf1 = GetUniqueLeaf(depth, current_index - 1);
        leaf2 = leaves_.Get(depth, current_index);
      }
      leaves_.Set(depth + 1, next_index,
                  HashLeftRight(leaf1, leaf2, kIVS[depth]));

      current_index = next_index;
    }
    return leaves_.Get(Levels, 0);
  }

  h256 GetUniqueLeaf(u128 depth, u128 offset) const {
    h256 leaf;
    if (!leaves_.Has(depth, offset)) {
      memcpy(leaf.data(), (byte *)&depth, sizeof(depth));
      memcpy(leaf.data() + sizeof(depth), (byte *)&offset, sizeof(offset));
      return leaf;
    }
    leaf = leaves_.Get(depth, offset);
    return leaf;
  }

  void SetCur() {
    platon_set_state((byte *)&kCurName, sizeof(kCurName), (byte *)&cur_,
                     sizeof(cur_));
  }
  void GetCur() {
    u128 cur = 0;
    platon_get_state((byte *)&kCurName, sizeof(kCurName), (byte *)&cur,
                     sizeof(cur));
    cur_ = cur;
  }

 private:
  class H256Matrix {
   public:
    void Set(u128 i, u128 j, const h256 &element) {
      std::array<byte, 40> key;
      CreateKey(i, j, key);
      platon_set_state(key.data(), key.size(), element.data(), element.size);
    }

    h256 Get(u128 i, u128 j) const {
      std::array<byte, 40> key;
      CreateKey(i, j, key);
      h256 element;
      platon_get_state(key.data(), key.size(), element.data(), element.size);
      return element;
    }

    bool Has(u128 i, u128 j) const {
      std::array<byte, 40> key;
      CreateKey(i, j, key);
      return platon_get_state_length(key.data(), key.size()) != 0;
    }

   private:
    void CreateKey(u128 i, u128 j, std::array<byte, 40> &key) const {
      uint64_t n = static_cast<uint64_t>(MerkleName);
      memcpy(key.data(), (byte *)&n, sizeof(uint64_t));
      memcpy(key.data() + sizeof(uint64_t), (byte *)&i, sizeof(u128));
      memcpy(key.data() + sizeof(uint64_t) + sizeof(u128), (byte *)&j,
             sizeof(u128));
    }
  };

  class RootMap {
   public:
    void Add(const h256 &root) {
      std::array<byte, 40> key;
      CreateKey(root, key);
      platon_set_state(key.data(), key.size(), &kExist, sizeof(kExist));
    }

    bool Has(const h256 &root) {
      std::array<byte, 40> key;
      CreateKey(root, key);
      return platon_get_state_length(key.data(), key.size()) != 0;
    }

   private:
    void CreateKey(const h256 &root, std::array<byte, 40> &key) {
      uint64_t n = static_cast<uint64_t>(MerkleName);
      memcpy(key.data(), (byte *)&n, sizeof(uint64_t));
      memcpy(key.data() + sizeof(uint64_t), root.data(), root.size);
    }

    const byte kExist = 1;
  };

 private:
  // hash(i) = sha3(hash(i-1)), hash(0) = h256{}
  constexpr static std::array<h256, 32> kIVS = {
      h256(std::string_view("0x290decd9548b62a8d60345a988386fc84ba6bc95484008f6"
                            "362f93160ef3e563")),
      h256(std::string_view("0x510e4e770828ddbf7f7b00ab00a9f6adaf81c0dc9cc85f1f"
                            "8249c256942d61d9")),
      h256(std::string_view("0x356e5a2cc1eba076e650ac7473fccc37952b46bc2e419a20"
                            "0cec0c451dce2336")),
      h256(std::string_view("0xb903bd7696740696b2b18bd1096a2873bb8ad0c2e7f25b00"
                            "a0431014edb3f539")),
      h256(std::string_view("0xf2e59013a0a379837166b59f871b20a8a0d101d1c355ea85"
                            "d35329360e69c000")),
      h256(std::string_view("0x582b06447f087674bcc0a32a19961e77dafb9e17955792f7"
                            "9ec8936e3d9742fc")),
      h256(std::string_view("0x6d29f6dd1270e49744bd5377ec86395b2de2abbe54bae162"
                            "81b8e39b35538dcd")),
      h256(std::string_view("0xc307e5620454e771052a14bea2468aa36e4d01104c1bb058"
                            "9fb96c692da7cebc")),
      h256(std::string_view("0xec9d541d1c32bec7debceb8103337e6ad3b1140e80ae7773"
                            "a1013fd6f3014c46")),
      h256(std::string_view("0x9b2baad7528ecec612c5751a6bd525905892d7892e155c3b"
                            "05e61363154a940b")),
      h256(std::string_view("0xd705bfceb18862841d146b65702167152de74c08a4c1821a"
                            "1698fcc414d8978e")),
      h256(std::string_view("0x4db8c3d89b7de6ddb733a736d664ebda3b6c5c5131f406df"
                            "463e8f83d7805283")),
      h256(std::string_view("0x9b1ca7a86d339a3f175e48937848ac1cc012678e046b8c43"
                            "3a226d06f0f9bf9b")),
      h256(std::string_view("0x526ee6e688a9b3d466fe4a7355197a89898593509d75c570"
                            "942cc73a5ca8299d")),
      h256(std::string_view("0x3db89d407a5318223df61aeabb86646833530dcccfb8fecb"
                            "21b4e19d6494d7a0")),
      h256(std::string_view("0xf1af2c862779f5efe4ec2ba0c3982e07c705fd93f1ce622b"
                            "03681128c04f6aa2")),
      h256(std::string_view("0x1c43434ae099df696d682e68d0611cc9b9eda636466134d6"
                            "e857c86c4bd6ed28")),
      h256(std::string_view("0xaec7cfe5efe3d3e0665826a4817adef725e254509263560c"
                            "f98f8a906172999d")),
      h256(std::string_view("0xc6320e3c1c3456002aa6ccc5b60cf5bf054d7e9392712f8b"
                            "7764869abe630035")),
      h256(std::string_view("0x4a5c69f25c2f9aa90e476c86b0a67af98a13d4b69fe70364"
                            "32b965132d340a76")),
      h256(std::string_view("0xcdf42a47e552f9b4eb01d44a7b6a2b057fbfa1070f3148fd"
                            "8e7f937a417ffdc1")),
      h256(std::string_view("0x77e29faaecdca7314f4aca32b59b79ba53423722779514f8"
                            "0382fe5fa4042b2a")),
      h256(std::string_view("0xdbfb8ad16703c762d244e3ca63bec236b7f967036c3345a5"
                            "d82136629df656e7")),
      h256(std::string_view("0x81c436d08902c24b1da0e87d3c82bbf16a4ac075c05398af"
                            "34d78bb664755fcf")),
      h256(std::string_view("0x6a6513eeb3ff2d755c71dc4a6d372e6a86bc4298a8328ee5"
                            "bf42aea3b23a85f2")),
      h256(std::string_view("0xb15bb3623092c525165d45efa2d08540708d90d033aaa9fe"
                            "4be8fd16ff904df0")),
      h256(std::string_view("0xc4e19b439d86b4e7cee42d39fbeb4b73364a36cef575d4a7"
                            "ca649f0869c9d79c")),
      h256(std::string_view("0xfeb48ef8a34c571d7939e39ca53a4386c7bb7c8170133438"
                            "ab923b0f16e9df52")),
      h256(std::string_view("0x920a075f9d5202e2cdf7cd984490fddef5218a4e6b6ce577"
                            "6ebdecee9129b7a7")),
      h256(std::string_view("0x1842cb89c0dfad7efb5ce2bf93f418e369214e9e1773a1bf"
                            "e7c9c72ceee682bb")),
      h256(std::string_view("0x7e7d8bef9d86983accafa937aed08042391d6f435bc640e5"
                            "0a9c38927de9b299")),
      h256(std::string_view("0x5a46dd85298b0beff65ccf3006c4b5d2f7fa6ca8a5885a7f"
                            "2132714fe7a48602"))};

  const uint64_t kCurName = static_cast<uint64_t>(MerkleName) + "cur"_n.value;

  u128 max_leaf_count_;
  u128 cur_;
  H256Matrix leaves_;
  RootMap rootMap_;
};

}  // namespace merkle
}  // namespace platon
