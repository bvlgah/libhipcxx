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

// UNSUPPORTED: c++98, c++03, c++11
#include <hip/std/functional>
// #include <hip/std/string>

template <class T>
struct is_transparent
{
private:
    struct two {char lx; char lxx;};
    template <class U> __host__ __device__ static two test(...);
    template <class U> __host__ __device__ static char test(typename U::is_transparent* = 0);
public:
    static const bool value = sizeof(test<T>(0)) == 1;
};


int main(int, char**)
{
    static_assert ( !is_transparent<hip::std::plus<int>>::value, "" );
    // static_assert ( !is_transparent<hip::std::plus<hip::std::string>>::value, "" );
    static_assert (  is_transparent<hip::std::plus<void>>::value, "" );
    static_assert (  is_transparent<hip::std::plus<>>::value, "" );

    static_assert ( !is_transparent<hip::std::minus<int>>::value, "" );
    // static_assert ( !is_transparent<hip::std::minus<hip::std::string>>::value, "" );
    static_assert (  is_transparent<hip::std::minus<void>>::value, "" );
    static_assert (  is_transparent<hip::std::minus<>>::value, "" );

    static_assert ( !is_transparent<hip::std::multiplies<int>>::value, "" );
    // static_assert ( !is_transparent<hip::std::multiplies<hip::std::string>>::value, "" );
    static_assert (  is_transparent<hip::std::multiplies<void>>::value, "" );
    static_assert (  is_transparent<hip::std::multiplies<>>::value, "" );

    static_assert ( !is_transparent<hip::std::divides<int>>::value, "" );
    // static_assert ( !is_transparent<hip::std::divides<hip::std::string>>::value, "" );
    static_assert (  is_transparent<hip::std::divides<void>>::value, "" );
    static_assert (  is_transparent<hip::std::divides<>>::value, "" );

    static_assert ( !is_transparent<hip::std::modulus<int>>::value, "" );
    // static_assert ( !is_transparent<hip::std::modulus<hip::std::string>>::value, "" );
    static_assert (  is_transparent<hip::std::modulus<void>>::value, "" );
    static_assert (  is_transparent<hip::std::modulus<>>::value, "" );

    static_assert ( !is_transparent<hip::std::negate<int>>::value, "" );
    // static_assert ( !is_transparent<hip::std::negate<hip::std::string>>::value, "" );
    static_assert (  is_transparent<hip::std::negate<void>>::value, "" );
    static_assert (  is_transparent<hip::std::negate<>>::value, "" );

    return 0;
}