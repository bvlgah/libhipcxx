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

#include <hip/std/mdspan>
#include <hip/std/cassert>

constexpr auto dyn = hip::std::dynamic_extent;

int main(int, char**)
{
    typedef int    data_t ;
    typedef size_t index_t;

    hip::std::array<data_t, 1> d{42};

    {
        hip::std::mdspan<data_t, hip::std::dextents<index_t,1>> m;

        static_assert( m.rank        () == 1, "" );
        assert       ( m.rank_dynamic() == 1 );
    }

    {
        hip::std::mdspan<data_t, hip::std::extents<index_t, 16>> m{d.data()};

        static_assert( m.rank        () == 1, "" );
        assert       ( m.rank_dynamic() == 0 );
    }

    {
        hip::std::mdspan<data_t, hip::std::extents<index_t, dyn, dyn>> m{d.data(), 16, 32};

        static_assert( m.rank        () == 2, "" );
        assert       ( m.rank_dynamic() == 2 );
    }

    {
        hip::std::mdspan<data_t, hip::std::extents<index_t, 8, dyn, dyn>> m{d.data(), 16, 32};

        static_assert( m.rank        () == 3, "" );
        assert       ( m.rank_dynamic() == 2 );
    }

    return 0;
}