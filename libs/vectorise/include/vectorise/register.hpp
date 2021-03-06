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

#include "vectorise/info.hpp"

#include <iostream>
#include <type_traits>
#include <typeinfo>

// clang-format off
// NOLINTNEXTLINE
#define APPLY_OPERATOR_LIST(FUNCTION) \
  FUNCTION(*)                         \
  FUNCTION(/)                         \
  FUNCTION(+)                         \
  FUNCTION(-)                         \
  FUNCTION(&)                         \
  FUNCTION(|)                         \
  FUNCTION (^)
// clang-format on

namespace fetch {
namespace vectorize {

template <typename T, std::size_t N = sizeof(T)>
class VectorRegister
{
public:
  using type             = T;
  using mm_register_type = T;

  enum
  {
    E_VECTOR_SIZE   = sizeof(mm_register_type),
    E_REGISTER_SIZE = sizeof(mm_register_type),
    E_BLOCK_COUNT   = E_REGISTER_SIZE / sizeof(type)
  };

  static_assert((E_BLOCK_COUNT * sizeof(type)) == E_REGISTER_SIZE,
                "type cannot be contained in the given register size.");

  VectorRegister()
  {}
  VectorRegister(type const *d)
    : data_(*d)
  {}
  VectorRegister(type const &d)
    : data_(d)
  {}
  VectorRegister(type &&d)
    : data_(d)
  {}

  explicit operator T()
  {
    return data_;
  }

  template <typename G>
  static G dsp_sum(G const *a, std::size_t const &n)
  {
    G ret(0);
    for (std::size_t i = 0; i < n; ++i)
    {
      ret += a[i];
    }
    return ret;
  }

  template <typename G>
  static G dsp_sum_of_product(G const *a, G const *b, std::size_t const &n)
  {
    T ret(0);
    for (std::size_t i = 0; i < n; ++i)
    {
      ret += a[i] * b[i];
    }
    return ret;
  }

#define FETCH_ADD_OPERATOR(OP)                            \
  VectorRegister operator OP(VectorRegister const &other) \
  {                                                       \
    return VectorRegister(type(data_ OP other.data_));    \
  }
  APPLY_OPERATOR_LIST(FETCH_ADD_OPERATOR);
#undef FETCH_ADD_OPERATOR

  void Store(type *ptr) const
  {
    *ptr = data_;
  }

private:
  type data_;
};

#undef APPLY_OPERATOR_LIST
}  // namespace vectorize
}  // namespace fetch
