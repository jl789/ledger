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

#include "ml/ops/embeddings.hpp"
#include "core/fixed_point/fixed_point.hpp"
#include "math/tensor.hpp"
#include <gtest/gtest.h>

template <typename T>
class EmbeddingsTest : public ::testing::Test
{
};

using MyTypes = ::testing::Types<fetch::math::Tensor<int>, fetch::math::Tensor<float>,
                                 fetch::math::Tensor<double>,
                                 fetch::math::Tensor<fetch::fixed_point::FixedPoint<16, 16>>,
                                 fetch::math::Tensor<fetch::fixed_point::FixedPoint<32, 32>>>;
TYPED_TEST_CASE(EmbeddingsTest, MyTypes);

TYPED_TEST(EmbeddingsTest, forward_shape)
{
  using SizeType = typename TypeParam::SizeType;
  fetch::ml::ops::Embeddings<TypeParam> e(SizeType(100), SizeType(60));
  TypeParam                             input(std::vector<uint64_t>({10}));
  for (unsigned int i(0); i < 10; ++i)
  {
    input.At(i) = typename TypeParam::Type(i);
  }
  TypeParam output = e.fetch::ml::template Ops<TypeParam>::Forward(
      std::vector<std::reference_wrapper<TypeParam const>>({input}));

  ASSERT_EQ(output.shape(), std::vector<typename TypeParam::SizeType>({10, 60}));
}

TYPED_TEST(EmbeddingsTest, forward)
{
  fetch::ml::ops::Embeddings<TypeParam> e(10, 6);

  TypeParam weights(std::vector<uint64_t>({10, 6}));

  for (unsigned int i(0); i < 10; ++i)
  {
    for (unsigned int j(0); j < 6; ++j)
    {
      weights.Set(i, j, typename TypeParam::Type(i * 10 + j));
    }
  }

  e.SetData(weights);
  TypeParam input(std::vector<uint64_t>({2}));
  input.At(0)      = typename TypeParam::Type(3);
  input.At(1)      = typename TypeParam::Type(5);
  TypeParam output = e.fetch::ml::template Ops<TypeParam>::Forward(
      std::vector<std::reference_wrapper<TypeParam const>>({input}));

  ASSERT_EQ(output.shape(), std::vector<typename TypeParam::SizeType>({2, 6}));

  std::vector<int> gt{30, 31, 32, 33, 34, 35, 50, 51, 52, 53, 54, 55};
  std::cout << " ---- " << std::endl;
  for (unsigned int i{0}; i < 2; ++i)
  {
    for (unsigned int j{0}; j < 6; ++j)
    {
      EXPECT_EQ(output.At(i, j), typename TypeParam::Type(gt[(i * 6) + j]));
      if (output.At(i, j) != (typename TypeParam::Type(gt[(i * 6) + j])))
      {
        std::cerr << "ERROR: " << output.At(i, j) << " "
                  << typename TypeParam::Type(gt[(i * 6) + j]) << std::endl;
        exit(-1);
      }
    }
  }
}

TYPED_TEST(EmbeddingsTest, backward)
{
  fetch::ml::ops::Embeddings<TypeParam> e(10, 6);
  TypeParam                             weights(std::vector<uint64_t>({10, 6}));
  for (unsigned int i(0); i < 10; ++i)
  {
    for (unsigned int j(0); j < 6; ++j)
    {
      weights.Set(i, j, typename TypeParam::Type(i * 10 + j));
    }
  }
  e.SetData(weights);

  TypeParam input(std::vector<uint64_t>({2}));
  input.At(0)      = typename TypeParam::Type(3);
  input.At(1)      = typename TypeParam::Type(5);
  TypeParam output = e.fetch::ml::template Ops<TypeParam>::Forward(
      std::vector<std::reference_wrapper<TypeParam const>>({input}));

  TypeParam errorSignal(std::vector<uint64_t>({2, 6}));
  for (unsigned int j(0); j < 2; ++j)
  {
    for (unsigned int k{0}; k < 6; ++k)
    {
      errorSignal.Set(j, k, typename TypeParam::Type((j * 6) + k));
    }
  }
  e.Backward({input}, errorSignal);
  e.Step(typename TypeParam::Type(1));

  output = e.fetch::ml::template Ops<TypeParam>::Forward(
      std::vector<std::reference_wrapper<TypeParam const>>({input}));
  std::vector<int> gt{30, 30, 30, 30, 30, 30, 44, 44, 44, 44, 44, 44};

  for (unsigned int j(0); j < 2; ++j)
  {
    for (unsigned int k{0}; k < 6; ++k)
    {
      EXPECT_EQ(output.At(j, k), typename TypeParam::Type(gt[(j * 6) + k]));
    }
  }
}
