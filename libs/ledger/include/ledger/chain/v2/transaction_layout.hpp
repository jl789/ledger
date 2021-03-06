#pragma once
//------------------------------------------------------------------------------
//
//   Copyright 2018-2019 Fetch.AI Limited
//
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
//
//------------------------------------------------------------------------------

#include "core/byte_array/const_byte_array.hpp"
#include "miner/optimisation/bitvector.hpp"

namespace fetch {
namespace ledger {
namespace v2 {

class Transaction;

/**
 * A Transaction Layout is a summary class that extracts certain subset of information
 * from a transaction. This minimal set of information should be useful only for the mining/packing
 * of transactions into blocks.
 */
class TransactionLayout
{
public:
  using ConstByteArray = byte_array::ConstByteArray;
  using ShardMask      = bitmanip::BitVector;
  using TokenAmount    = uint64_t;
  using BlockIndex     = uint64_t;

  // Construction / Destruction
  TransactionLayout() = default;
  explicit TransactionLayout(Transaction const &tx);
  TransactionLayout(TransactionLayout const &) = default;
  TransactionLayout(TransactionLayout &&)      = default;
  ~TransactionLayout()                         = default;

  /// @name Accessors
  /// @{
  ConstByteArray const &digest() const;
  ShardMask const &     mask() const;
  TokenAmount           charge() const;
  BlockIndex            valid_from() const;
  BlockIndex            valid_until() const;
  /// @}

  // Operators
  TransactionLayout &operator=(TransactionLayout const &) = default;
  TransactionLayout &operator=(TransactionLayout &&) = default;

private:
  ConstByteArray digest_{};
  ShardMask      mask_{};
  TokenAmount    charge_{0};
  BlockIndex     valid_from_{0};
  BlockIndex     valid_until_{0};

  // Native serializers
  template <typename T>
  friend void Serialize(T &s, TransactionLayout const &tx);
  template <typename T>
  friend void Deserialize(T &s, TransactionLayout &tx);
};

/**
 * Get the associated transaction digest
 *
 * @return The transaction digest
 */
inline TransactionLayout::ConstByteArray const &TransactionLayout::digest() const
{
  return digest_;
}

/**
 * Get the shard mask usage for this transaction
 *
 * @return The shard mask
 */
inline TransactionLayout::ShardMask const &TransactionLayout::mask() const
{
  return mask_;
}

/**
 * Get the charge (fee) associated with the transaction
 *
 * @return The charge amount
 */
inline TransactionLayout::TokenAmount TransactionLayout::charge() const
{
  return charge_;
}

/**
 * The block index from which point the transaction is valid
 *
 * @return The block index from when the block is valid
 */
inline TransactionLayout::BlockIndex TransactionLayout::valid_from() const
{
  return valid_from_;
}

/**
 * The block index until which the transaction is valid
 *
 * @return THe block index from which the transaction becomes invalid
 */
inline TransactionLayout::BlockIndex TransactionLayout::valid_until() const
{
  return valid_until_;
}

}  // namespace v2
}  // namespace ledger
}  // namespace fetch
