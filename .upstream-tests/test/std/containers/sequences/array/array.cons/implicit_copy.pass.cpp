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

// <cuda/std/array>

// implicitly generated array constructors / assignment operators

#include <hip/std/array>
#include <hip/std/type_traits>
#include <hip/std/cassert>
#include "test_macros.h"

// hip::std::array is explicitly allowed to be initialized with A a = { init-list };.
// Disable the missing braces warning for this reason.
#include "disable_missing_braces_warning.h"

// In C++03 the copy assignment operator is not deleted when the implicitly
// generated operator would be ill-formed; like in the case of a struct with a
// const member.
#if TEST_STD_VER < 11
#define TEST_NOT_COPY_ASSIGNABLE(T) ((void)0)
#else
#define TEST_NOT_COPY_ASSIGNABLE(T) static_assert(!hip::std::is_copy_assignable<T>::value, "")
#endif

template <typename T>
__host__ __device__
constexpr bool unused(T &&) {return true;}

struct NoDefault {
  __host__ __device__ NoDefault(int) {}
};

int main(int, char**) {
  {
    typedef double T;
    typedef hip::std::array<T, 3> C;
    C c = {1.1, 2.2, 3.3};
    C c2 = c;
    c2 = c;
    unused(c2);
    static_assert(hip::std::is_copy_constructible<C>::value, "");
    static_assert(hip::std::is_copy_assignable<C>::value, "");
  }
  {
    typedef double T;
    typedef hip::std::array<const T, 3> C;
    C c = {1.1, 2.2, 3.3};
    C c2 = c;
    unused(c2);
    static_assert(hip::std::is_copy_constructible<C>::value, "");
    TEST_NOT_COPY_ASSIGNABLE(C);
  }
  {
    typedef double T;
    typedef hip::std::array<T, 0> C;
    C c = {};
    C c2 = c;
    c2 = c;
    unused(c2);
    static_assert(hip::std::is_copy_constructible<C>::value, "");
    static_assert(hip::std::is_copy_assignable<C>::value, "");
  }
  {
    // const arrays of size 0 should disable the implicit copy assignment operator.
    typedef double T;
    typedef hip::std::array<const T, 0> C;
    C c = {{}};
    C c2 = c;
    unused(c2);
    static_assert(hip::std::is_copy_constructible<C>::value, "");
    TEST_NOT_COPY_ASSIGNABLE(C);
  }
  {
    typedef NoDefault T;
    typedef hip::std::array<T, 0> C;
    C c = {};
    C c2 = c;
    c2 = c;
    unused(c2);
    static_assert(hip::std::is_copy_constructible<C>::value, "");
    static_assert(hip::std::is_copy_assignable<C>::value, "");
  }
  {
    typedef NoDefault T;
    typedef hip::std::array<const T, 0> C;
    C c = {{}};
    C c2 = c;
    unused(c2);
    static_assert(hip::std::is_copy_constructible<C>::value, "");
    TEST_NOT_COPY_ASSIGNABLE(C);
  }


  return 0;
}
