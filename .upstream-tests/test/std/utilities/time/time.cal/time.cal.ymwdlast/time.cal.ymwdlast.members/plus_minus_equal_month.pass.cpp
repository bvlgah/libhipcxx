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
// class year_month_weekday_last;

// constexpr year_month_weekday_last& operator+=(const months& m) noexcept;
// constexpr year_month_weekday_last& operator-=(const months& m) noexcept;

#include <hip/std/chrono>
#include <hip/std/type_traits>
#include <cassert>

#include "test_macros.h"

template <typename D, typename Ds>
__host__ __device__
constexpr bool testConstexpr(D d1)
{
    if (static_cast<unsigned>((d1          ).month()) !=  1) return false;
    if (static_cast<unsigned>((d1 += Ds{ 1}).month()) !=  2) return false;
    if (static_cast<unsigned>((d1 += Ds{ 2}).month()) !=  4) return false;
    if (static_cast<unsigned>((d1 += Ds{12}).month()) !=  4) return false;
    if (static_cast<unsigned>((d1 -= Ds{ 1}).month()) !=  3) return false;
    if (static_cast<unsigned>((d1 -= Ds{ 2}).month()) !=  1) return false;
    if (static_cast<unsigned>((d1 -= Ds{12}).month()) !=  1) return false;
    return true;
}

int main(int, char**)
{
    using year                    = hip::std::chrono::year;
    using month                   = hip::std::chrono::month;
    using weekday                 = hip::std::chrono::weekday;
    using weekday_last            = hip::std::chrono::weekday_last;
    using year_month_weekday_last = hip::std::chrono::year_month_weekday_last;
    using months                  = hip::std::chrono::months;

    ASSERT_NOEXCEPT(hip::std::declval<year_month_weekday_last&>() += std::declval<months>());
    ASSERT_NOEXCEPT(hip::std::declval<year_month_weekday_last&>() -= std::declval<months>());

    ASSERT_SAME_TYPE(year_month_weekday_last&, decltype(hip::std::declval<year_month_weekday_last&>() += std::declval<months>()));
    ASSERT_SAME_TYPE(year_month_weekday_last&, decltype(hip::std::declval<year_month_weekday_last&>() -= std::declval<months>()));

    constexpr weekday Tuesday = hip::std::chrono::Tuesday;
    static_assert(testConstexpr<year_month_weekday_last, months>(year_month_weekday_last{year{1234}, month{1}, weekday_last{Tuesday}}), "");

    for (unsigned i = 0; i <= 10; ++i)
    {
        year y{1234};
        year_month_weekday_last ymwd(y, month{i}, weekday_last{Tuesday});

        assert(static_cast<unsigned>((ymwd += months{2}).month()) == i + 2);
        assert(ymwd.year()     == y);
        assert(ymwd.weekday()  == Tuesday);

        assert(static_cast<unsigned>((ymwd             ).month()) == i + 2);
        assert(ymwd.year()     == y);
        assert(ymwd.weekday()  == Tuesday);

        assert(static_cast<unsigned>((ymwd -= months{1}).month()) == i + 1);
        assert(ymwd.year()     == y);
        assert(ymwd.weekday()  == Tuesday);

        assert(static_cast<unsigned>((ymwd             ).month()) == i + 1);
        assert(ymwd.year()     == y);
        assert(ymwd.weekday()  == Tuesday);
    }

  return 0;
}