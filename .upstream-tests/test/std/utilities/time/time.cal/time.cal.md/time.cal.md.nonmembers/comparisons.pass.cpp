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
// class month_day;

// constexpr bool operator==(const month_day& x, const month_day& y) noexcept;
//   Returns: x.month() == y.month() && x.day() == y.day().
//
// constexpr bool operator< (const month_day& x, const month_day& y) noexcept;
//   Returns:
//      If x.month() < y.month() returns true.
//      Otherwise, if x.month() > y.month() returns false.
//      Otherwise, returns x.day() < y.day().

#include <hip/std/chrono>
#include <hip/std/type_traits>
#include <cassert>

#include "test_macros.h"
#include "test_comparisons.h"

int main(int, char**)
{
    using day       = hip::std::chrono::day;
    using month     = hip::std::chrono::month;
    using month_day = hip::std::chrono::month_day;

    AssertComparisons6AreNoexcept<month_day>();
    AssertComparisons6ReturnBool<month_day>();

    static_assert( testComparisons6(
        month_day{hip::std::chrono::January, day{1}},
        month_day{hip::std::chrono::January, day{1}},
        true, false), "");

    static_assert( testComparisons6(
        month_day{hip::std::chrono::January, day{1}},
        month_day{hip::std::chrono::January, day{2}},
        false, true), "");

    static_assert( testComparisons6(
        month_day{hip::std::chrono::January,  day{1}},
        month_day{hip::std::chrono::February, day{1}},
        false, true), "");

//  same day, different months
    for (unsigned i = 1; i < 12; ++i)
        for (unsigned j = 1; j < 12; ++j)
            assert((testComparisons6(
                month_day{month{i}, day{1}},
                month_day{month{j}, day{1}},
                i == j, i < j )));

//  same month, different days
    for (unsigned i = 1; i < 31; ++i)
        for (unsigned j = 1; j < 31; ++j)
            assert((testComparisons6(
                month_day{month{2}, day{i}},
                month_day{month{2}, day{j}},
                i == j, i < j )));


  return 0;
}