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
// class year_month_day;

//  year_month_day() = default;
//  constexpr year_month_day(const chrono::year& y, const chrono::month& m,
//                                   const chrono::day& d) noexcept;
//
//  Effects:  Constructs an object of type year_month_day by initializing
//                y_ with y, m_ with m, and d_ with d.
//
//  constexpr chrono::year   year() const noexcept;
//  constexpr chrono::month month() const noexcept;
//  constexpr chrono::day     day() const noexcept;
//  constexpr bool             ok() const noexcept;

#include <hip/std/chrono>
#include <hip/std/type_traits>
#include <cassert>

#include "test_macros.h"

int main(int, char**)
{
    using year           = hip::std::chrono::year;
    using month          = hip::std::chrono::month;
    using day            = hip::std::chrono::day;
    using year_month_day = hip::std::chrono::year_month_day;

    ASSERT_NOEXCEPT(year_month_day{});
    ASSERT_NOEXCEPT(year_month_day{year{1}, month{1}, day{1}});

    constexpr month January = hip::std::chrono::January;

    constexpr year_month_day ym0{};
    static_assert( ym0.year()  == year{},  "");
    static_assert( ym0.month() == month{}, "");
    static_assert( ym0.day()   == day{},   "");
    static_assert(!ym0.ok(),               "");

    constexpr year_month_day ym1{year{2019}, January, day{12}};
    static_assert( ym1.year()  == year{2019}, "");
    static_assert( ym1.month() == January,    "");
    static_assert( ym1.day()   == day{12},    "");
    static_assert( ym1.ok(),                  "");


  return 0;
}
