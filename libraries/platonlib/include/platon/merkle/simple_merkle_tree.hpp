#pragma once
#include "platon/assert.hpp"
#include "platon/common.h"
#include "platon/event.hpp"
#include "platon/name.hpp"

#include <span>

namespace platon {
namespace merkle {

/**
 * @brief Merkle tree simple version implementation

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
class SimpleMerkleTree {
  PLATON_EVENT0(NewLeafEvent, u128, const h256 &, const h256 &);
  PLATON_EVENT0(NewLeavesEvent, u128, const std::vector<h256> &, const h256 &);
  PLATON_EVENT0(OutputEvent, const h256 &, const h256 &, const h256 &, u128,
                u128);

 public:
  SimpleMerkleTree() {
    static_assert(Levels > 1 && Levels <= 32);
    GetLeafCount();
  }
  SimpleMerkleTree(const SimpleMerkleTree &) = delete;
  SimpleMerkleTree &operator=(const SimpleMerkleTree &) = delete;

  ~SimpleMerkleTree() { SetLeafCount(); }

  /**
   * @brief Insert leaf node, create new leaf event
   *
   * @param leaf_value leaf node hash value
   * @return new root hash
   *
   */
  h256 InsertLeaf(h256 leaf_value) {
    platon_assert(leaf_count_ < kTreeWidth);
    u128 slot = GetFrontierSlot(leaf_count_);
    u128 node_index = leaf_count_ + kTreeWidth - 1;
//    u128 prev_node_index = 0;
    h256 node_value = leaf_value;

    for (u128 level = 0; level < kTreeHeight; level++) {
      if (level == slot) frontier_.Set(slot, node_value);
      if (node_index % 2 == 0) {
        node_value = HashLeftRight(frontier_.Get(level), node_value);
//        prev_node_index = node_index;
        node_index = (node_index - 1) / 2;
      } else {
        node_value = HashLeftRight(node_value, kZero);
        prev_node_index = node_index;
//        node_index = node_index / 2;
      }
    }

    PLATON_EMIT_EVENT0(NewLeafEvent, leaf_count_, leaf_value, node_value);
    leaf_count_++;
    return node_value;
  }

  /**
   * @brief  multiple leaves into the Merkle Tree, create new leaves event
   *
   * @param leaf_values leaves node hash value
   * @return new root hash
   *
   */
  h256 InsertLeaves(const std::vector<h256> &leaf_values) {
    platon_assert(leaf_count_ + leaf_values.size() < kTreeWidth);
    u128 slot = 0;
    u128 node_index = 0;
    //    u128 prev_node_index = 0;
    h256 node_value;

    for (u128 leaf_index = leaf_count_;
         leaf_index < leaf_count_ + leaf_values.size(); leaf_index++) {
      node_value = leaf_values[leaf_index - leaf_count_];
      node_index = leaf_index + kTreeWidth - 1;
      slot = GetFrontierSlot(leaf_index);
      if (slot == 0) {
        frontier_.Set(slot, node_value);
        continue;
      }

      for (u128 level = 1; level <= slot; level++) {
        if (node_index % 2 == 0) {
          node_value = HashLeftRight(frontier_.Get(level - 1), node_value);
          //          prev_node_index = node_index;
          node_index = (node_index - 1) / 2;
        } else {
          node_value = HashLeftRight(node_value, kZero);
          //          prev_node_index = node_index;
          node_index = node_index / 2;
        }
      }
      frontier_.Set(slot, node_value);
    }

    for (u128 level = slot + 1; level <= kTreeHeight; level++) {
      if (node_index % 2 == 0) {
        node_value = HashLeftRight(frontier_.Get(level - 1), node_value);
        //        prev_node_index = node_index;
        node_index = (node_index - 1) / 2;
      } else {
        node_value = HashLeftRight(node_value, kZero);
        //        prev_node_index = node_index;
        node_index = node_index / 2;
      }
    }

    PLATON_EMIT_EVENT0(NewLeavesEvent, leaf_count_, leaf_values, node_value);

    leaf_count_ += leaf_values.size();
    return node_value;
  }

  u128 LeafCount() const { return leaf_count_; }

  h256 FrontierValue(u128 index) { return frontier_.Get(index); }

 private:
  u128 GetFrontierSlot(u128 leaf_index) const {
    u128 slot = 0;
    if (leaf_index % 2 == 1) {
      u128 exp1 = 1;
      u128 pow1 = 2;
      u128 pow2 = pow1 << 1;
      while (slot == 0) {
        if ((leaf_index + 1 - pow1) % pow2 == 0) {
          slot = exp1;
        } else {
          pow1 = pow2;
          pow2 = pow2 << 1;
          exp1++;
        }
      }
    }
    return slot;
  }

  h256 HashLeftRight(const h256 &left, const h256 &right) {
    std::array<const h256 *, 2> array = {&left, &right};
    return H::Hash(array);
  }

  void SetLeafCount() {
    platon_set_state((byte *)&kLeafCountName, sizeof(kLeafCountName),
                     (byte *)&leaf_count_, sizeof(leaf_count_));
  }

  void GetLeafCount() {
    u128 leaf_count = 0;
    platon_get_state((byte *)&kLeafCountName, sizeof(kLeafCountName),
                     (byte *)&leaf_count, sizeof(leaf_count));
    leaf_count_ = leaf_count;
  }

 private:
  class Frontier {
   public:
    void Set(u128 i, const h256 &element) {
      std::array<byte, 24> key;
      CreateKey(i, key);
      platon_set_state(key.data(), key.size(), element.data(), element.size);
    }

    h256 Get(u128 i) {
      std::array<byte, 24> key;
      CreateKey(i, key);
      h256 element;
      platon_get_state(key.data(), key.size(), element.data(), element.size);
      return element;
    }

   private:
    void CreateKey(u128 i, std::array<byte, 24> &key) {
      uint64_t n = static_cast<uint64_t>(MerkleName);
      memcpy(key.data(), (byte *)&n, sizeof(uint64_t));
      memcpy(key.data() + sizeof(uint64_t), (byte *)&i, sizeof(u128));
    }
  };

 private:
  const u128 kTreeHeight = Levels;
  const u128 kTreeWidth = 1ULL << Levels;
  const h256 kZero = h256{};
  const uint64_t kLeafCountName =
      static_cast<uint64_t>(MerkleName) + "leaf"_n.value;
  u128 leaf_count_ = 0;
  Frontier frontier_;
};
}  // namespace merkle
}  // namespace platon
