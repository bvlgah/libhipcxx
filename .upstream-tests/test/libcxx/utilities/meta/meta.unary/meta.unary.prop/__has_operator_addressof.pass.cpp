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

// UNSUPPORTED: c++98, c++03

// type_traits

// extension

// template <typename _Tp> struct __has_operator_addressof


#include <hip/std/type_traits>

#include "test_macros.h"


struct A
{
};

struct B
{
    __host__ __device__
    constexpr B* operator&() const;
};

struct D;

struct C
{
    template <class U>
    __host__ __device__
    D operator,(U&&);
};

struct E
{
    __host__ __device__
    constexpr C operator&() const;
};

struct F {};
__host__ __device__
constexpr F* operator&(F const &) { return nullptr; }

struct G {};
__host__ __device__
constexpr G* operator&(G &&) { return nullptr; }

struct H {};
__host__ __device__
constexpr H* operator&(H const &&) { return nullptr; }

struct J
{
    __host__ __device__
    constexpr J* operator&() const &&;
};


int main(int, char**)
{
    static_assert(hip::std::__has_operator_addressof<int>::value == false, "");
    static_assert(hip::std::__has_operator_addressof<A>::value == false, "");
    static_assert(hip::std::__has_operator_addressof<B>::value == true, "");
    static_assert(hip::std::__has_operator_addressof<E>::value == true, "");
    static_assert(hip::std::__has_operator_addressof<F>::value == true, "");
    static_assert(hip::std::__has_operator_addressof<G>::value == true, "");
    static_assert(hip::std::__has_operator_addressof<H>::value == true, "");
    static_assert(hip::std::__has_operator_addressof<J>::value == true, "");

  return 0;
}
