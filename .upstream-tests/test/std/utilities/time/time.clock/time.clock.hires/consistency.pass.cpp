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
//
// Due to C++17 inline variables ASAN flags this test as containing an ODR
// violation because Clock::is_steady is defined in both the dylib and this TU.
// UNSUPPORTED: asan

// Starting with C++17, Clock::is_steady is inlined (but not before LLVM-3.9!),
// but before C++17 it requires the symbol to be present in the dylib, which
// is only shipped starting with macosx10.9.
// XFAIL: with_system_cxx_lib=macosx10.7 && (c++98 || c++03 || c++11 || c++14 || apple-clang-7 || apple-clang-8.0)
// XFAIL: with_system_cxx_lib=macosx10.8 && (c++98 || c++03 || c++11 || c++14 || apple-clang-7 || apple-clang-8.0)

// <cuda/std/chrono>

// high_resolution_clock

// check clock invariants

#include <hip/std/chrono>

template <class T>
__host__ __device__
void test(const T &) {}

int main(int, char**)
{
    typedef hip::std::chrono::high_resolution_clock C;
    static_assert((hip::std::is_same<C::rep, C::duration::rep>::value), "");
    static_assert((hip::std::is_same<C::period, C::duration::period>::value), "");
    static_assert((hip::std::is_same<C::duration, C::time_point::duration>::value), "");
    static_assert(C::is_steady || !C::is_steady, "");
    test(+hip::std::chrono::high_resolution_clock::is_steady);

  return 0;
}