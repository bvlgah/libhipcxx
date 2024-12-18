//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// Modifications Copyright (c) 2024 Advanced Micro Devices, Inc.
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// UNSUPPORTED: c++98, c++03, c++11, nvrtc

// <chrono>
// class weekday;

// constexpr weekday operator+(const days& x, const weekday& y) noexcept;
//   Returns: weekday(int{x} + y.count()).
//
// constexpr weekday operator+(const weekday& x, const days& y) noexcept;
//   Returns:
//      weekday{modulo(static_cast<long long>(unsigned{x}) + y.count(), 7)}
//   where modulo(n, 7) computes the remainder of n divided by 7 using Euclidean division.
//   [Note: Given a divisor of 12, Euclidean division truncates towards negative infinity
//   and always produces a remainder in the range of [0, 6].
//   Assuming no overflow in the signed summation, this operation results in a weekday
//   holding a value in the range [0, 6] even if !x.ok(). —end note]
//   [Example: Monday + days{6} == Sunday. —end example]



#include <hip/std/chrono>
#include <hip/std/type_traits>
#include <cassert>

#include "test_macros.h"
#include "../../euclidian.h"

template <typename M, typename Ms>
__host__ __device__
constexpr bool testConstexpr()
{
    M m{1};
    Ms offset{4};
    if (m + offset != M{5}) return false;
    if (offset + m != M{5}) return false;
//  Check the example
    if (M{1} + Ms{6} != M{0}) return false;
    return true;
}

int main(int, char**)
{
    using weekday = hip::std::chrono::weekday;
    using days    = hip::std::chrono::days;

    ASSERT_NOEXCEPT(                   std::declval<weekday>() + std::declval<days>());
    ASSERT_SAME_TYPE(weekday, decltype(hip::std::declval<weekday>() + std::declval<days>()));

    ASSERT_NOEXCEPT(                   std::declval<days>() + std::declval<weekday>());
    ASSERT_SAME_TYPE(weekday, decltype(hip::std::declval<days>() + std::declval<weekday>()));

    static_assert(testConstexpr<weekday, days>(), "");

    for (unsigned i = 0; i <= 6; ++i)
        for (unsigned j = 0; j <= 6; ++j)
        {
            weekday wd1 = weekday{i} + days{j};
            weekday wd2 = days{j} + weekday{i};
            assert(wd1 == wd2);
            assert((wd1.c_encoding() == euclidian_addition<unsigned, 0, 6>(i, j)));
            assert((wd2.c_encoding() == euclidian_addition<unsigned, 0, 6>(i, j)));
        }

  return 0;
}
