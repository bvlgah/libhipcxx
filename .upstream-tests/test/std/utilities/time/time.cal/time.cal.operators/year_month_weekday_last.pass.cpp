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

// constexpr year_month_weekday_last
//   operator/(const year_month& ym, const weekday_last& wdl) noexcept;
// Returns: {ym.year(), ym.month(), wdl}.
//
// constexpr year_month_weekday_last
//   operator/(const year& y, const month_weekday_last& mwdl) noexcept;
// Returns: {y, mwdl.month(), mwdl.weekday_last()}.
//
// constexpr year_month_weekday_last
//   operator/(int y, const month_weekday_last& mwdl) noexcept;
// Returns: year(y) / mwdl.
//
// constexpr year_month_weekday_last
//   operator/(const month_weekday_last& mwdl, const year& y) noexcept;
// Returns: y / mwdl.
//
// constexpr year_month_weekday_last
//   operator/(const month_weekday_last& mwdl, int y) noexcept;
// Returns: year(y) / mwdl.



#include <hip/std/chrono>
#include <hip/std/type_traits>
#include <cassert>

#include "test_macros.h"
#include "test_comparisons.h"

int main(int, char**)
{
    using year_month         = hip::std::chrono::year_month;
    using year               = hip::std::chrono::year;
    using month              = hip::std::chrono::month;
    using weekday            = hip::std::chrono::weekday;
    using weekday_last       = hip::std::chrono::weekday_last;
    using month_weekday_last = hip::std::chrono::month_weekday_last;
    using year_month_weekday_last = hip::std::chrono::year_month_weekday_last;

    constexpr weekday Tuesday = hip::std::chrono::Tuesday;
    constexpr month February = hip::std::chrono::February;

    { // operator/(const year_month& ym, const weekday_last& wdl) (and switched)
        constexpr year_month Feb2018{year{2018}, February};

        ASSERT_NOEXCEPT (                                  Feb2018/weekday_last{Tuesday});
        ASSERT_SAME_TYPE(year_month_weekday_last, decltype(Feb2018/weekday_last{Tuesday}));

        static_assert((Feb2018/weekday_last{Tuesday}).year()    == year{2018}, "");
        static_assert((Feb2018/weekday_last{Tuesday}).month()   == February,   "");
        static_assert((Feb2018/weekday_last{Tuesday}).weekday() == Tuesday,    "");

        for (int i = 1000; i < 1010; ++i)
            for (unsigned j = 1; j <= 12; ++j)
                for (unsigned k = 0; k <= 6; ++k)
                {
                    year y{i};
                    month m{j};
                    weekday wd{k};
                    year_month_weekday_last ymwdl = year_month{y,m}/weekday_last{wd};
                    assert(ymwdl.year()    == y);
                    assert(ymwdl.month()   == m);
                    assert(ymwdl.weekday() == wd);
                }
    }


    { // operator/(const year& y, const month_weekday_last& mwdl) (and switched)
        constexpr month_weekday_last FebLastTues{February, weekday_last{Tuesday}};

        ASSERT_NOEXCEPT (                                  year{2018}/FebLastTues);
        ASSERT_SAME_TYPE(year_month_weekday_last, decltype(year{2018}/FebLastTues));
        ASSERT_NOEXCEPT (                                  FebLastTues/year{2018});
        ASSERT_SAME_TYPE(year_month_weekday_last, decltype(FebLastTues/year{2018}));


        static_assert((year{2018}/FebLastTues).year()    == year{2018}, "");
        static_assert((year{2018}/FebLastTues).month()   == February,   "");
        static_assert((year{2018}/FebLastTues).weekday() == Tuesday,    "");
        static_assert((FebLastTues/year{2018}).year()    == year{2018}, "");
        static_assert((FebLastTues/year{2018}).month()   == February,   "");
        static_assert((FebLastTues/year{2018}).weekday() == Tuesday,    "");


        for (int i = 1000; i < 1010; ++i)
            for (unsigned j = 1; j <= 12; ++j)
                for (unsigned k = 0; k <= 6; ++k)
                {
                    year y{i};
                    month m{j};
                    weekday wd{k};
                    year_month_weekday_last ymwdl1 = y/month_weekday_last{m, weekday_last{wd}};
                    year_month_weekday_last ymwdl2 = month_weekday_last{m, weekday_last{wd}}/y;
                    assert(ymwdl1.year()    == y);
                    assert(ymwdl2.year()    == y);
                    assert(ymwdl1.month()   == m);
                    assert(ymwdl2.month()   == m);
                    assert(ymwdl1.weekday() == wd);
                    assert(ymwdl2.weekday() == wd);
                    assert(ymwdl1 == ymwdl2);
                }
    }


    { // operator/(int y, const month_weekday_last& mwdl) (and switched)
        constexpr month_weekday_last FebLastTues{February, weekday_last{Tuesday}};

        ASSERT_NOEXCEPT (                                  2018/FebLastTues);
        ASSERT_SAME_TYPE(year_month_weekday_last, decltype(2018/FebLastTues));
        ASSERT_NOEXCEPT (                                  FebLastTues/2018);
        ASSERT_SAME_TYPE(year_month_weekday_last, decltype(FebLastTues/2018));


        static_assert((2018/FebLastTues).year()    == year{2018}, "");
        static_assert((2018/FebLastTues).month()   == February,   "");
        static_assert((2018/FebLastTues).weekday() == Tuesday,    "");
        static_assert((FebLastTues/2018).year()    == year{2018}, "");
        static_assert((FebLastTues/2018).month()   == February,   "");
        static_assert((FebLastTues/2018).weekday() == Tuesday,    "");


        for (int i = 1000; i < 1010; ++i)
            for (unsigned j = 1; j <= 12; ++j)
                for (unsigned k = 0; k <= 6; ++k)
                {
                    year y{i};
                    month m{j};
                    weekday wd{k};
                    year_month_weekday_last ymwdl1 = i/month_weekday_last{m, weekday_last{wd}};
                    year_month_weekday_last ymwdl2 = month_weekday_last{m, weekday_last{wd}}/i;
                    assert(ymwdl1.year()    == y);
                    assert(ymwdl2.year()    == y);
                    assert(ymwdl1.month()   == m);
                    assert(ymwdl2.month()   == m);
                    assert(ymwdl1.weekday() == wd);
                    assert(ymwdl2.weekday() == wd);
                    assert(ymwdl1 == ymwdl2);
                }
    }

  return 0;
}
