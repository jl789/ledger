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

#include "ml/ops/activations/elu.hpp"
#include "core/fixed_point/fixed_point.hpp"
#include "math/tensor.hpp"
#include <gtest/gtest.h>

template <typename T>
class EluTest : public ::testing::Test
{
};

using MyTypes = ::testing::Types<fetch::math::Tensor<float>, fetch::math::Tensor<double>,
                                 fetch::math::Tensor<fetch::fixed_point::FixedPoint<32, 32>>>;

TYPED_TEST_CASE(EluTest, MyTypes);

TYPED_TEST(EluTest, forward_test)
{
  using DataType  = typename TypeParam::Type;
  using ArrayType = TypeParam;

  ArrayType           data(8);
  ArrayType           gt(8);
  std::vector<double> dataInput({1, -2, 3, -4, 5, -6, 7, -8});
  std::vector<double> gtInput(
      {1, -1.72932943352677, 3, -1.96336872222253, 5, -1.99504249564667, 7, -1.99932907474419});
  for (std::uint64_t i(0); i < 8; ++i)
  {
    data.Set(i, DataType(dataInput[i]));
    gt.Set(i, DataType(gtInput[i]));
  }
  fetch::ml::ops::Elu<ArrayType> op(DataType(2.0));
  TypeParam                      prediction = op.fetch::ml::template Ops<TypeParam>::Forward(
      std::vector<std::reference_wrapper<TypeParam const>>({data}));

  // test correct values
  ASSERT_TRUE(
      prediction.AllClose(gt, typename TypeParam::Type(1e-5), typename TypeParam::Type(1e-5)));
}

TYPED_TEST(EluTest, backward_test)
{
  using DataType  = typename TypeParam::Type;
  using ArrayType = TypeParam;

  ArrayType           data(8);
  ArrayType           error(8);
  ArrayType           gt(8);
  std::vector<double> dataInput({1, -2, 3, -4, 5, -6, 7, -8});
  std::vector<double> errorInput({0, 0, 0, 0, 1, 0, 0, 0});
  std::vector<double> gtInput({0, 0, 0, 0, 1, 0, 0, 0});
  for (std::uint64_t i(0); i < 8; ++i)
  {
    data.Set(i, DataType(dataInput[i]));
    error.Set(i, DataType(errorInput[i]));
    gt.Set(i, DataType(gtInput[i]));
  }
  fetch::ml::ops::Elu<ArrayType> op(DataType(2.0));
  std::vector<ArrayType>         prediction = op.Backward({data}, error);

  // test correct values
  ASSERT_TRUE(prediction[0].AllClose(gt, DataType(1e-5), DataType(1e-5)));
}
