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
// class month;

// constexpr month operator+(const month& x, const months& y) noexcept;
//   Returns: month(int{x} + y.count()).
//
// constexpr month operator+(const months& x, const month& y) noexcept;
//   Returns:
//     month{modulo(static_cast<long long>(int{x}) + (y.count() - 1), 12) + 1}
//   where modulo(n, 12) computes the remainder of n divided by 12 using Euclidean division.
//   [Note: Given a divisor of 12, Euclidean division truncates towards negative infinity
//   and always produces a remainder in the range of [0, 11].
//   Assuming no overflow in the signed summation, this operation results in a month
//   holding a value in the range [1, 12] even if !x.ok(). —end note]
//   [Example: February + months{11} == January. —end example]



#include <hip/std/chrono>
#include <hip/std/type_traits>
#include <cassert>

#include "test_macros.h"

template <typename M, typename Ms>
__host__ __device__
constexpr bool testConstexpr()
{
    M m{1};
    Ms offset{4};
    if (m + offset != M{5}) return false;
    if (offset + m != M{5}) return false;
//  Check the example
    if (M{2} + Ms{11} != M{1}) return false;
    return true;
}

int main(int, char**)
{
    using month  = hip::std::chrono::month;
    using months = hip::std::chrono::months;

    ASSERT_NOEXCEPT(hip::std::declval<month>() + std::declval<months>());
    ASSERT_NOEXCEPT(hip::std::declval<months>() + std::declval<month>());

    ASSERT_SAME_TYPE(month, decltype(hip::std::declval<month>()  + std::declval<months>()));
    ASSERT_SAME_TYPE(month, decltype(hip::std::declval<months>() + std::declval<month>() ));

    static_assert(testConstexpr<month, months>(), "");

    month my{2};
    for (unsigned i = 0; i <= 15; ++i)
    {
        month m1 = my + months{i};
        month m2 = months{i} + my;
        assert(m1 == m2);
        unsigned exp = i + 2;
        while (exp > 12)
            exp -= 12;
        assert(static_cast<unsigned>(m1) == exp);
        assert(static_cast<unsigned>(m2) == exp);
    }

  return 0;
}
