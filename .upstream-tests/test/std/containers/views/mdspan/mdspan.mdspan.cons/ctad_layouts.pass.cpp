//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

//UNSUPPORTED: c++11, nvrtc && nvcc-12.0, nvrtc && nvcc-12.1

// No CTAD in C++14 or earlier
//UNSUPPORTED: c++14

#include <hip/std/mdspan>
#include <hip/std/cassert>

#define CHECK_MDSPAN(m,d,exhaust,s0,s1) \
    static_assert(m.rank()          == 2 , ""); \
    static_assert(m.rank_dynamic()  == 2 , ""); \
    assert(m.data_handle()   == d.data()); \
    assert(m.extent(0)       == 16      ); \
    assert(m.extent(1)       == 32      ); \
    assert(m.stride(0)       == s0      ); \
    assert(m.stride(1)       == s1      ); \
    assert(m.is_exhaustive() == exhaust )


int main(int, char**)
{
#ifdef __MDSPAN_USE_CLASS_TEMPLATE_ARGUMENT_DEDUCTION
    // TEST(TestMdspanCTAD, layout_left)
    {
        hip::std::array<int, 1> d{42};
        hip::std::mdspan m0{d.data(), hip::std::layout_left::mapping{hip::std::extents{16, 32}}};

        CHECK_MDSPAN( m0, d, true, 1, 16 );
    }

    // TEST(TestMdspanCTAD, layout_right)
    {
        hip::std::array<int, 1> d{42};
        hip::std::mdspan m0{d.data(), hip::std::layout_right::mapping{hip::std::extents{16, 32}}};

        CHECK_MDSPAN( m0, d, true, 32, 1 );
    }

    // TEST(TestMdspanCTAD, layout_stride)
    {
        hip::std::array<int, 1> d{42};
        hip::std::mdspan m0{d.data(), hip::std::layout_stride::mapping{hip::std::extents{16, 32}, hip::std::array{1, 128}}};

        CHECK_MDSPAN( m0, d, false, 1, 128 );
    }
#endif

    return 0;
}